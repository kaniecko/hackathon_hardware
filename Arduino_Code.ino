#include "dht.h"
#define dht_apin A3 // Analog Pin sensor is connected to
dht DHT;

#include<SoftwareSerial.h>
SoftwareSerial ArduinoUno(3,2);

// pins
const int ph_sensor = A0;
const int water_level_1 = A1;
const int water_level_2 = A2;
const int outletPin = 8;

// System info
float current_ph = 7.0;
float low_ph = 0.0;
float up_ph = 14.0;
bool overflow = false;
bool underflow = false;
bool turbulance = false;
unsigned long last_t_h_reading = 0;
float temperature = 69.42;
float humidity = 12.5;
bool pump_status = false;
bool over_ride_pump_status = false;
unsigned long pump_interval_seconds = 15;//30 * 60;
unsigned long last_pump_on_time = 0;
bool light_status = false;
int light_duration_minutes = 60;
String all_info = "";

void setup() {
  Serial.begin(9600);
  ArduinoUno.begin(4800);
  Serial.println("");
  Serial.println("System has started!");
  pinMode(outletPin, OUTPUT);
  digitalWrite(outletPin, LOW);
  delay(1500);
}

void loop() {
  read_ph();
  read_temperature_humidity();
  water_level_check();
  send_all_info_to_string();
  Serial.println("");
  delay(1000);
}

void send_all_info_to_string() {
  all_info = 
  String(current_ph) + "," +
  String(low_ph) + "," +
  String(up_ph) + "," +
  String(overflow) + "," +
  String(underflow) + "," +
  String(turbulance) + "," +
  String(temperature) + "," +
  String(humidity) + "," +
  String(pump_status) + "," +
  String(pump_interval_seconds) + "," +
  String(light_status) + "," +
  String(light_duration_minutes) + ",";
  ArduinoUno.write(all_info.c_str());
  Serial.println(all_info);
}

void read_temperature_humidity() {
  if ((millis() - last_t_h_reading) > 5000) {
    DHT.read11(dht_apin);
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    humidity = DHT.humidity;
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    temperature = DHT.temperature;
    Serial.println("C  ");
    last_t_h_reading = millis();
  }
}

void turbulance_check() {
  int water_level = analogRead(water_level_2);
  turbulance = false;
}

void water_level_check() {
  int water_level = analogRead(water_level_1);
  if ((millis() - last_pump_on_time) > pump_interval_seconds * 1000) {
    if (water_level < 100) {
      underflow = true;
      overflow = false;
    }
    if (water_level < 400) {
      update_water_pump(true);
      overflow = false;
      underflow = true;
      Serial.println("Pump turned on");
    }
    if (water_level > 400) {
      update_water_pump(false);
      overflow = true;
      underflow = false;
      Serial.println("Pump turned off");
    }
    Serial.println("water_level: " + String(water_level));
    last_pump_on_time = millis();
  } else {
    if (water_level > 400) {
      update_water_pump(false);
      overflow = true;
      underflow = false;
      Serial.println("Pump turned off");
    }
  }
}

void read_ph() {
  float total = 0;
  for(int i = 0; i < 10;i++){
    total += analogRead(ph_sensor);
    delay(10);
  }
  current_ph = -5.70 * (total*5.0/1024.0/10.0) + 21.5;
  Serial.println("PH: " + String(current_ph));
}

void update_water_pump(bool state) {
  if (over_ride_pump_status) {
    state = false;
  }  
  pump_status = state;
  if (state) {
    digitalWrite(outletPin, HIGH);
  } else {
    digitalWrite(outletPin, LOW);
  }
}
