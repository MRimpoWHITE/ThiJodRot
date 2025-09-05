#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 

int ledPin = D2;
int analogPin = A0;
int val = 0;
int buzz = 2;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(buzz, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  val = analogRead(analogPin); // อ่านค่าสัญญาณ analog ขา A0
  
  // แปลงค่าจาก analog (100–900) ไปเป็นระยะ (80–10 cm)
  int distance = map(val, 100, 900, 80, 10);

  // ป้องกันค่าเกินช่วง
  if (distance < 10) distance = 10;
  if (distance > 80) distance = 80;

  Serial.print("Analog = ");
  Serial.print(val);
  Serial.print("  Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 20)  // ถ้าใกล้กว่า 20 cm
  {
    digitalWrite(ledPin, HIGH); // LED ติด
    digitalWrite(buzz, LOW);    // Buzzer ติด
  }
  else
  {
    digitalWrite(ledPin, LOW);  // LED ดับ
    digitalWrite(buzz, HIGH);   // Buzzer ดับ
  }
  delay(100);
}
