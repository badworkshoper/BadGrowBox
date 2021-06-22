#include <GyverOS.h>
#define PumpPin 13
#define SoilPin A0
#define HumPin A2
GyverOS<3> OS;

uint32_t watering_time = 5000;
uint32_t watering_time_2 = 5000;
uint32_t watering_timer = 0;
bool watering = false;

int wetting_time = 5000;
uint32_t wetting_timer = 0;
bool wetting_state = false;

int NUM_READINGS = 10;
int soil_moisture = 0;
int soil_moisture_2 = 2;

int soil_min = 620;
int soil_max = 900;



void setup() {
  // put your setup code here, to run once:
  OS.attach(0, readSoil, 1000);
  OS.attach(1, pumpRegulator, 10000);//43200000 - 12h
  OS.attach(2, wetting, 12000);
  Serial.begin(9600);
  pinMode(PumpPin, OUTPUT);
  pinMode(HumPin, OUTPUT);
}

void readSoil(){
  static int reads = 0;
  static int read_sum = 0;
  if(reads <= NUM_READINGS){
    read_sum += analogRead(SoilPin);
    reads++;
  }
  if(reads >= NUM_READINGS){
    soil_moisture = map(read_sum / NUM_READINGS, soil_min, soil_max, 100, 0);
    //Serial.println(soil_moisture);
    read_sum = 0;
    reads = 0;
  }
}

void wetting(){

  if(!wetting_state){
    wetting_state = true;
    analogWrite(HumPin, 1023);
    delay(100);
    analogWrite(HumPin, 0);
    wetting_timer = millis();
    Serial.println("Wetting on");
  }
}

void pumpRegulator(){
  watering = true;
  watering_timer = millis();
  digitalWrite(PumpPin, watering);
  
  //Serial.println("watering on");
}

void loop() {
  OS.tick();
  if(millis() - watering_timer >= watering_time){
    watering_timer = millis();
    if(digitalRead(PumpPin) == true){
      watering = false;
      digitalWrite(PumpPin, watering);
      //Serial.println("watering off");
    }
  }
  if(millis() - wetting_timer >= wetting_time){
    wetting_timer = millis();
    if(wetting_state){
      analogWrite(HumPin, 1023);
      delay(100);
      analogWrite(HumPin, 0);
      Serial.println("Wetting off");
      wetting_state = false;
    }
  }
  // put your main code here, to run repeatedly:
}
