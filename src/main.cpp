#include <Arduino.h> // Infrared photoelectric switch Sensor E18-D80NK 

int ledPin = D2;
int analogPin = A0;
int val = 0;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  val = analogRead(analogPin); // อ่านค่าสัญญาณ analog ขา A0


  // int distance = map(val, 100, 900, 80, 10);

  // if (distance < 10) distance = 10;
  // if (distance > 80) distance = 80;


  Serial.print("val = ");      // พิมพ์ข้อความส่งเข้าคอมพิวเตอร์ "val = "
  Serial.println(val);         // พิมพ์ค่าของตัวแปร val
  // Serial.print("  Distance = ");
  // Serial.print(distance);
  // Serial.println(" cm");

  if (val < 200)
  {
    digitalWrite(ledPin, HIGH); // สั่งให้ LED ติดสว่าง
    Serial.println("LED = HIGH");    
    delay(1500);
  }
  else
  {
    digitalWrite(ledPin, LOW); // สั่งให้ LED ดับ
  Serial.println("LED = LOW");     
  delay(1500); 
  }
  delay(1000);
}
