#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

const int sensorPin1 = D5;   // เซนเซอร์ขวา
const int sensorPin2 = D6;

//Led ของเซนเซอร์ ขวา
int Rsensor1 = D1;    
int Gsensor1 = D2;

int Rsensor2 = D7;
int Gsensor2 = D0;

//กันค่ารวน
int triggerCount1 = 0;
int triggerCount2 = 0;
const int triggerThreshold = 3; // ต้องเจอสัญญาณ 3 รอบติดกัน

// สถานะล่าสุด
bool lastState1 = false;
bool lastState2 = false;


void sendToAPI();

void setup() {
  pinMode(sensorPin1, INPUT_PULLUP); // sennor ขวา
  pinMode(sensorPin2, INPUT_PULLUP); // sennorอันแรก

  pinMode(Rsensor1, OUTPUT);
  pinMode(Gsensor1, OUTPUT);

  pinMode(Rsensor2, OUTPUT);
  pinMode(Gsensor2, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}



void loop() {

  bool s1 = (digitalRead(sensorPin1) == LOW);
  bool s2 = (digitalRead(sensorPin2) == LOW);
  


  if (s1 == LOW ) {           // เมื่อมีรถมาจอดไฟจะติด

    triggerCount1++;

    if(triggerCount1 >= triggerThreshold){
      analogWrite(Rsensor1, 255);
      analogWrite(Gsensor1, 0);

      if (lastState1 != true) {
        lastState1 = true;
        sendToAPI("1", true);  // ส่งเฉพาะตอนเปลี่ยน
        Serial.println("slot 1 Sensor : Detected");
      }
    }
  } 
  
  else {
    triggerCount1 = 0;
    analogWrite(Rsensor1, 0);
    analogWrite(Gsensor1, 255);
    if (lastState1 != false) {
      lastState1 = false;
      sendToAPI("1", false);
      Serial.println("slot 1 Sensor : CLEAR");
    }
  }


  if (s2 == LOW ) {           // เมื่อมีรถมาจอดไฟจะติด HIGH

    triggerCount2++;

    if(triggerCount2 >= triggerThreshold){
      analogWrite(Rsensor2, 255);
      analogWrite(Gsensor2, 0);
      if (lastState2 != true) {
        lastState2 = true;
        sendToAPI("2", true);
        Serial.println("slot 2 Sensor : Detected");
        Serial.println("====================");
      }
    }
  } 
    else {
    triggerCount2 = 0;

    analogWrite(Rsensor2, 0);
    analogWrite(Gsensor2, 255);
    if(lastState2 != false){
      lastState2 = false;
      sendToAPI("2", false);
    }
    Serial.println("slot 2 Sensor : CLEAR");
    Serial.println("====================");

  }


  delay(500);
}



void sendToAPI(String sensor, bool status) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // ข้ามการตรวจ SSL cert

    HTTPClient http;
    String url = "https://thijodrot.onrender.com/api/parking";

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    // JSON ที่จะส่ง
    String payload = "{\"slot\":\"" + slot + "\", \"status\":" + (status ? "true" : "false") + "}";

    int httpResponseCode = http.POST(payload);

    Serial.print("POST to API: ");
    Serial.println(payload);
    Serial.print("Response: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected, cannot send API");
  }
}