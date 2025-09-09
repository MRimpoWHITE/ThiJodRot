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

  //pinMode(ledPin, OUTPUT);
  pinMode(Rsensor2, OUTPUT);
  pinMode(Gsensor2, OUTPUT);

  Serial.begin(115200);

}

void loop() {

  bool s1 = (digitalRead(sensorPin1) == LOW);
  bool s2 = (digitalRead(sensorPin2) == LOW);
  


  if (s1 == HIGH ) {           // เมื่อมีรถมาจอดไฟจะติด          

    triggerCount1++;

    if(triggerCount1 >= triggerThreshold){
      analogWrite(Rsensor1, 255);
      analogWrite(Gsensor1, 0);
      Serial.println("slot 1 Sensor : Detected");
    }
  }
  
  else {
    triggerCount1 = 0;
    analogWrite(Rsensor1, 0);
    analogWrite(Gsensor1, 255);
    Serial.println("slot 1 Sensor : CLEAR");
  }


    
  if (s2 == HIGH ) {           // เมื่อมีรถมาจอดไฟจะติด

    triggerCount2++;

    if(triggerCount2 >= triggerThreshold){
      analogWrite(Rsensor2, 255);
      analogWrite(Gsensor2, 0);
      Serial.println("slot 2 Sensor : Detected");
      Serial.println("====================");
      
    }
  } 
    else {
    triggerCount2 = 0;

    analogWrite(Rsensor2, 0);
    analogWrite(Gsensor2, 255);
    Serial.println("slot 2 Sensor : CLEAR");
    Serial.println("====================");
    }

  delay(1000);
}



