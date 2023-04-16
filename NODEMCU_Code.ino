/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

const char* ssid = "KAiPhone";
const char* password = "kapi12345";

//Your Domain name with URL path or IP address with path
String server_path = "http://3.93.20.205/esp";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

SoftwareSerial nmcu8266(D2,D3);
String arduino_full_info = "!";

// System info
float current_ph = 7.0;
float low_ph = 0.0;
float up_ph = 14.0;
bool overflow = false;
bool underflow = false;
bool turbulance = false;
float temperature = 69.42;
float humidity = 12.5;
bool pump_status = false;
int pump_interval_seconds = 30 * 60;
bool light_status = false;
int light_duration_minutes = 60;

void setup() {
  Serial.begin(9600);
  nmcu8266.begin(4800);
  connect_WIFI();
}

void loop() {
  get_arduino_info();
  get_and_send_data();
  delay(50);
}

void get_arduino_info() {
  if (nmcu8266.available() > 0) {
    arduino_full_info = nmcu8266.readString();
    char c = '?';
    int i = 0;
    String temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    current_ph = temp.toFloat();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    low_ph = temp.toFloat();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    up_ph = temp.toFloat();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    overflow = bool(temp.toInt());
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    underflow = bool(temp.toInt());
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    turbulance = bool(temp.toInt());
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    temperature = temp.toFloat();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    humidity = temp.toFloat();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    pump_status = bool(temp.toInt());
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    pump_interval_seconds = temp.toInt();
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    light_status = bool(temp.toInt());
    temp = "";
    do {
      c = arduino_full_info[i];
      temp += c;
      i++;
    }while(c != ',');
    light_duration_minutes = temp.toInt();
  }
}

void connect_WIFI(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void get_and_send_data(){
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, server_path.c_str()); // begin connection
      http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // make sure its a string sending
      String responseBody = 
      "ph:" + String(current_ph) + ", " +
      "phLow:" + String(low_ph) + ", " +
      "phHigh:" + String(up_ph) + ", " +
      "overflow:" + String(overflow) + ", " +
      "underflow:" + String(underflow) + ", " +
      "turbulance:" + String(turbulance) + ", " +
      "temperature:" + String(temperature) + ", " +
      "humidity:" + String(humidity) + ", " +
      "pump_status:" + String(pump_status) + ", " +
      "pump_interval_seconds:" + String(pump_interval_seconds) + ", " +
      "light_status:" + String(light_status) + ", " +
      "light_duration_minutes:" + String(light_duration_minutes) + ", " +
      "";
      //Serial.println(responseBody);
      int httpResponseCode = http.POST(responseBody);
      if (httpResponseCode == 200) {
        Serial.println("ESP: " + responseBody);
        String payload = http.getString();
        Serial.println("Server: " + payload);
      }
      else {
        Serial.println("Error in communicating with server");
        Serial.println("Error code: " + String(httpResponseCode));
      }
      Serial.println("");
      http.end();      
    }
    else {
      Serial.println("WiFi Disconnected");
      connect_WIFI(); // reconnect if anything
    }
    lastTime = millis();
  }
}