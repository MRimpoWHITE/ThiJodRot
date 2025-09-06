#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 

const int sensor1 = D1;
const int sensor2 = D2;
const int led1 = D5;  // indicator for sensor1
const int led2 = D6;  // indicator for sensor2

void setup() {
  pinMode(sensor1, INPUT_PULLUP);
  pinMode(sensor2, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  bool s1 = (digitalRead(sensor1) == LOW);
  bool s2 = (digitalRead(sensor2) == LOW);

  digitalWrite(led1, s1 ? HIGH : LOW); // ถ้า LED ต่อแบบ anode->GPIO, cathode->GND
  digitalWrite(led2, s2 ? HIGH : LOW);

  Serial.print("S1:"); Serial.println(s1);
  Serial.print(" S2:"); Serial.println(s2);
  delay(100);
}


//+++++++++++++++++++++++++++++++++++++++++++++++



int triggerCount = 0;
const int triggerThreshold = 3; // ต้องเจอสัญญาณ 3 รอบติดกัน

void loop() {
  int s = digitalRead(sensorPin);
  if (s == LOW) { // triggered
    triggerCount++;
    if(triggerCount >= triggerThreshold){
      // มีวัตถุจริง
      digitalWrite(ledPin, LOW);
    }
  } else {
    triggerCount = 0;
    digitalWrite(ledPin, HIGH);
  }
  delay(50);
}
