#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wifi config
const char* ssid = "NetworkName";
const char* password = "NetworkPassword";
const char* server = "http://COMPUTER_IP:5000/data";

// Pins REMEBER TO SET UP YOUR PINS
const int POT1 = 0, POT2 = 0;
const int BTN1 = 0, BTN2 = 0;
const int LED1 = 0, LED2 = 0;

// State variables
bool btn1_state = false, btn2_state = false;
unsigned long last_pot_send = 0;
unsigned long last_debounce[2] = {0, 0};

void setup() {
  Serial.begin(115200);
  
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  handleButton(BTN1, LED1, btn1_state, last_debounce[0], 0);
  handleButton(BTN2, LED2, btn2_state, last_debounce[1], 1);
  
  if (millis() - last_pot_send > 500) {
    sendData();
    last_pot_send = millis();
  }
}

void handleButton(int pin, int led, bool &state, unsigned long &debounce, int id) {
  if (digitalRead(pin) == LOW && millis() - debounce > 150) {
    state = !state;
    digitalWrite(led, state);
    debounce = millis();
    
    StaticJsonDocument<200> doc;
    doc[id == 0 ? "button1_state" : "button2_state"] = state;
    sendJson(doc);
  }
}

void sendData() {
  int pot1 = analogRead(POT1);
  int pot2 = analogRead(POT2);
  
  StaticJsonDocument<200> doc;
  doc["pot1_raw"] = pot1;
  doc["pot1_percentage"] = map(pot1, 0, 4095, 0, 100);
  doc["pot2_raw"] = pot2;
  doc["pot2_percentage"] = map(pot2, 0, 4095, 0, 100);
  doc["button1_state"] = btn1_state;
  doc["button2_state"] = btn2_state;
  doc["timestamp"] = millis();
  
  sendJson(doc);
}

void sendJson(StaticJsonDocument<200> &doc) {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String json;
  serializeJson(doc, json);
  
  http.begin(server);
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  http.end();
}