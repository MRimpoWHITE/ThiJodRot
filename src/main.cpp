#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

const int sensorPin1 = D5;   // เซนเซอร์ขวา
const int sensorPin2 = D6;

//Led ของเซนเซอร์ ขวา
int Rsensor1 = D0;    
int Gsensor1 = D2;

int Rsensor2 = D7;
int Gsensor2 = D1;

//กันค่ารวน
int triggerCount1 = 0;
int triggerCount2 = 0;
const int triggerThreshold = 3; // ต้องเจอสัญญาณ 3 รอบติดกัน

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
      Serial.println("1.Right Senor : Detected");
    }
  } 
  
  else {
    triggerCount1 = 0;

    analogWrite(Rsensor1, 0);
    analogWrite(Gsensor1, 255);
    Serial.println("1.Right Senor : CLEAR");
  }


  if (s2 == LOW ) {           // เมื่อมีรถมาจอดไฟจะติด HIGH

    triggerCount2++;

    if(triggerCount2 >= triggerThreshold){
      analogWrite(Rsensor2, 255);
      analogWrite(Gsensor2, 0);
      Serial.println("2.Left Senor : Detected");
      Serial.println("====================");
    }
  } 
    else {
    triggerCount2 = 0;

    analogWrite(Rsensor2, 0);
    analogWrite(Gsensor2, 255);
    Serial.println("2.Left Senor : CLEAR");
    Serial.println("====================");

  }


  delay(500);
}
