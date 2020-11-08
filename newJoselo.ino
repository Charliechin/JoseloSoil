

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// =================
// Config
// =================

// ====== Wifi Config
char* ssid = "TNCAPC55F29"; // your network SSID (name)
char* password = "00B81D72EC";   // your network key

const char* host = "api.pushingbox.com";
const char* devId = "v465AF0FCD880F8B";
// =================

// ====== Sensor Pinout

byte pinMoistureSensor = A0;
byte pinMoistureVCC = 4; // D2

// ====== Sensor config
int dryValue = 5;
int wetValue = 540;

int friendlyDryValue = 0;
int friendlyWetValue = 100;
// =================

const boolean DEBUG = false;


void initWifi()
{
      WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
//    PrintText("Conectado al WiFi!",36,50);
//    PrintText(String(Wifi.localIP()),36,50);
    
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


int readMoisture() {
  digitalWrite(pinMoistureVCC, HIGH); // power up sensor
  delay(500);
  
  int value = analogRead(pinMoistureSensor);
  digitalWrite(pinMoistureVCC, LOW); // power down sensor

  Serial.println(map(1023 - value, dryValue, wetValue, friendlyDryValue, friendlyWetValue));
  
  return map(1023 - value, dryValue, wetValue, friendlyDryValue, friendlyWetValue);
 
}


void sendNotification() {

    Serial.print("connecting to ");
    Serial.println(host);
    
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/pushingbox";
    url += "?devid=";
    url += devId;
    
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
    Serial.println();
    Serial.println();
    Serial.println("closing connection");
}





// ================= SETUP & LOOP

void setup() {
  Serial.begin(115200);
  initWifi();
  pinMode(pinMoistureSensor, INPUT);
  pinMode(pinMoistureVCC, OUTPUT);
  digitalWrite(pinMoistureVCC, LOW);    // by default, we do not power the sensor

}

void loop() {

  if(readMoisture() < 25 && !DEBUG){

    sendNotification();
    //   cada 180 mins, estoy seca
    delay(10000 * 180);
    
    
  }else{
 
    Serial.print("New Value: ");
    Serial.println(readMoisture() );
    delay(100);
  }

}
