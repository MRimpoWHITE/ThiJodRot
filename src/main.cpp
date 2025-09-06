#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 

const int sensorPin1 = D5;   // เซนเซอร์ขวา
const int sensorPin2 = D5;

//Led ของเซนเซอร์ ขวา
int Rsensor1 = D4;    
int Gsensor1 = D2;

int Rsensor2 = D2;
int Gsensor2 = D4;

//กันค่ารวน
int triggerCount1 = 0;
int triggerCount2 = 0;
const int triggerThreshold = 3; // ต้องเจอสัญญาณ 3 รอบติดกัน

void setup() {
  pinMode(sensorPin1, INPUT_PULLUP); // sennor ขวา
  pinMode(sensorPin2, INPUT_PULLUP); // sennorอันแรก


  pinMode(Rsensor1, OUTPUT);
  pinMode(Gsensor1, OUTPUT);

  //pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {

  analogWrite(Rsensor1, 0);
  analogWrite(Gsensor1, 0);

  bool s1 = (digitalRead(sensorPin1) == LOW);
  bool s2 = (digitalRead(sensorPin2) == LOW);

  // digitalWrite(led1, s1 ? HIGH : LOW); // ถ้า LED ต่อแบบ anode->GPIO, cathode->GND
  // digitalWrite(led2, s2 ? HIGH : LOW);


  if (s1 == HIGH ) {           // เมื่อมีรถมาจอดไฟจะติด

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

  Serial.println("====================");

  if (s2 == HIGH ) {           // เมื่อมีรถมาจอดไฟจะติด

    triggerCount2++;

    if(triggerCount2 >= triggerThreshold){
      analogWrite(Rsensor2, 255);
      analogWrite(Gsensor2, 0);
      Serial.println("2.Left Senor : Detected");
    }
  } 
    else {
    triggerCount2 = 0;

    analogWrite(Rsensor2, 0);
    analogWrite(Gsensor2, 255);
    Serial.println("2.Left Senor : CLEAR");

  }

  Serial.println("====================");

  delay(1000);
}
