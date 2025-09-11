#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "S1-211"; 
const char* password = "68718224";

// API
const char* api_post = "https://thijodrot.onrender.com/api/parking";
const char* api_get  = "https://thijodrot.onrender.com/api/parking/status";

// Sensor
const int sensorPin1 = D5;
const int sensorPin2 = D6;

// LED
int Rsensor1 = D1;
int Gsensor1 = D2;

int Rsensor2 = D7;
int Gsensor2 = D0;

// กันค่ารวน
int triggerCount1 = 0;
int triggerCount2 = 0;
const int triggerThreshold = 3;

// สถานะล่าสุด
bool lastState1 = false;
bool lastState2 = false;

// ซ่อมบำรุง (บังคับจาก server)
bool override1 = false;
bool override2 = false;

// เวลาสุดท้ายที่ดึง config
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 2000; // 1 วิ

// ===== ส่ง API =====
void sendToAPI(int slot, int status) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    http.begin(client, api_post);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"slot\":" + String(slot) + ", \"status\":" + String(status) + "}";
    int httpResponseCode = http.POST(payload);

    Serial.print("POST Data: ");
    Serial.println(payload);
    Serial.print("Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println("Response Body: " + response);

    http.end();
  }
}

// ===== ดึงค่า config จาก server =====
void fetchConfig() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    http.begin(client, api_get);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("==== Raw JSON ====");
      Serial.println(response);
      Serial.println("==================");

      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error) {
        // เพราะ server ส่งมาเป็น array
        for (JsonObject obj : doc.as<JsonArray>()) {
          int slot   = obj["slot"];
          int status = obj["status"];

          if (slot == 1) {
            override1 = (status == 2);
          } else if (slot == 2) {
            override2 = (status == 2);
          }
        }
      } else {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Config GET failed, code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}



// ===== Setup =====
void setup() {
  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);

  pinMode(Rsensor1, OUTPUT);
  pinMode(Gsensor1, OUTPUT);
  pinMode(Rsensor2, OUTPUT);
  pinMode(Gsensor2, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// ===== Loop =====
 
  void loop() {

 // ดึง config ทุก 2 วิ
  if (millis() - lastFetch > fetchInterval) {
    lastFetch = millis();
    fetchConfig();
    Serial.print(override1);
    Serial.println(override2);
  }

  bool s1 = (digitalRead(sensorPin1) == HIGH);
  bool s2 = (digitalRead(sensorPin2) == HIGH);

  // ---------- Slot 1 ----------
  if (override1) {
    // ซ่อมบำรุง → เหลือง
    analogWrite(Rsensor1, 255);
    analogWrite(Gsensor1, 100);
  } else {
    // ตรวจจับ
    if (s1) {
      triggerCount1++;
      if (triggerCount1 >= triggerThreshold && lastState1 == false) {
        lastState1 = true;
        triggerCount1 = 0; // reset กัน overshoot
        analogWrite(Rsensor1, 255);
        analogWrite(Gsensor1, 0);
        Serial.println("slot 1 Sensor : Detected");
        sendToAPI(1, 1);
      }
    } else {
      if (lastState1 == true) {
        lastState1 = false;
        triggerCount1 = 0;
        analogWrite(Rsensor1, 0);
        analogWrite(Gsensor1, 255);
        Serial.println("slot 1 Sensor : CLEAR");
        sendToAPI(1, 0);
      }
    }
  }

  // ---------- Slot 2 ----------
  if (override2) {
    // ซ่อมบำรุง → เหลือง
    analogWrite(Rsensor2, 255);
    analogWrite(Gsensor2, 100);
  } else {
    // ตรวจจับ
    if (s2) {
      triggerCount2++;
      if (triggerCount2 >= triggerThreshold && lastState2 == false) {
        lastState2 = true;
        triggerCount2 = 0;
        analogWrite(Rsensor2, 255);
        analogWrite(Gsensor2, 0);
        Serial.println("slot 2 Sensor : Detected");
        sendToAPI(2, 1);
      }
    } else {
      if (lastState2 == true) {
        lastState2 = false;
        triggerCount2 = 0;
        analogWrite(Rsensor2, 0);
        analogWrite(Gsensor2, 255);
        Serial.println("slot 2 Sensor : CLEAR");
        sendToAPI(2, 0);
      }
    }
  }

  delay(50); // ลดให้ responsive มากขึ้น
}

