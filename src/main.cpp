#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "realmec67"; 
const char* password = "12345678";

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
const unsigned long fetchInterval = 10000; // 10 วิ

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
      Serial.println("Config Response: " + response);

      DynamicJsonDocument doc(256);
      if (!deserializeJson(doc, response)) {
        int slot   = doc["slot"];   // 1 หรือ 2
        int status = doc["status"]; // 0=ว่าง, 1=มีรถ, 2=ซ่อม

        if (slot == 1) {
          override1 = (status == 2);
        } else if (slot == 2) {
          override2 = (status == 2);
        }
        else{
          override1 = false;
          override2 = false;
        }
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
  // ดึง config ทุก 10 วิ
  if (millis() - lastFetch > fetchInterval) {
    lastFetch = millis();
    fetchConfig();
  }

  bool s1 = (digitalRead(sensorPin1) == HIGH);
  bool s2 = (digitalRead(sensorPin2) == HIGH);

  // ---------- Slot 1 ----------
  if (override1) {
    analogWrite(Rsensor1, 150);
    analogWrite(Gsensor1, 255); // สีเหลือง
    Serial.println("slot 1 Sensor : MAINTENANCE");
  } else {
    if (s1) {
      triggerCount1++;
      if (triggerCount1 >= triggerThreshold) {
        analogWrite(Rsensor1, 255);
        analogWrite(Gsensor1, 0);
        if (lastState1 != true) {
          lastState1 = true;
          sendToAPI(1, 1);
          Serial.println("slot 1 Sensor : Detected");
        }
      }
    } else {
      triggerCount1 = 0;
      analogWrite(Rsensor1, 0);
      analogWrite(Gsensor1, 255);
      if (lastState1 != false) {
        lastState1 = false;
        sendToAPI(1, 0);
        Serial.println("slot 1 Sensor : CLEAR");
      }
    }
  }

  // ---------- Slot 2 ----------
  if (override2) {
    analogWrite(Rsensor2, 150);
    analogWrite(Gsensor2, 255); // สีเหลือง
    Serial.println("slot 2 Sensor : MAINTENANCE");
  } else {
    if (s2) {
      triggerCount2++;
      if (triggerCount2 >= triggerThreshold) {
        analogWrite(Rsensor2, 255);
        analogWrite(Gsensor2, 0);
        if (lastState2 != true) {
          lastState2 = true;
          sendToAPI(2, 1);
          Serial.println("slot 2 Sensor : Detected");
          Serial.println("====================");
        }
      }
    } else {
      triggerCount2 = 0;
      analogWrite(Rsensor2, 0);
      analogWrite(Gsensor2, 255);
      if (lastState2 != false) {
        lastState2 = false;
        sendToAPI(2, 0);
        Serial.println("slot 2 Sensor : CLEAR");
        Serial.println("====================");
      }
    }
  }

  delay(200);
}
