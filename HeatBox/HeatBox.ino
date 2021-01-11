/*
 Name:    HeatBox.ino
 Created: 07.05.2020 22:29:01
 Author:  badworkshoper
*/

// the setup function runs once when you press reset or power the board

// libraries
#include <Wire.h>
#include <U8glib.h>
#include <GyverEncoder.h>
#include <EEPROM.h>


#define refTEMPAdress 0
#define hystTEMPAdress 4




//some defines
   //number of readings for avarage
#define heatRELE 2      //pin for heating relay
//encoder
#define SW 9
#define DT 10
#define CLK 11
#define STEPCHANGE 0.25  //step changing slow

int16_t readdelay = 500;
uint8_t i = 0;
uint8_t menu = 0;
unsigned long time_now = 0;
uint8_t readcount = 10;
float refTEMP = 29.50;  //referance temperature
float TEMP1SUM = 0;     //sum for temperature calculation
float TEMP1 = 23.50;    // Temperature
float hystTEMP = 0.5;     // hysteresis for temperature

bool redraw = false;
bool heating = false;


U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);

//Termistor block
int ThermistorPin = A0;
int Vo;
float R1 = 10000; // value of R1 on board
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;
//Termistor block

void setup() {
  enc.setType(TYPE2);
  Serial.begin(9600);
  u8g.setFont(u8g_font_unifontr);
  pinMode(heatRELE, OUTPUT); 
  int testvalue = 0;
  EEPROM.get(0, testvalue);
  if(testvalue == -1){
    StoreParams();
  }

}


void StoreParams(){
    EEPROM.put(refTEMPAdress, refTEMP);
    EEPROM.put(hystTEMPAdress, hystTEMP);
}

void temp_regulator() {
   
    if (TEMP1 <= refTEMP - hystTEMP * 0.5) {
        heating = true;
        digitalWrite(heatRELE, heating);
    }
    if (TEMP1 >= refTEMP + hystTEMP * 0.5) {
        heating = false;
        digitalWrite(heatRELE, heating);
    }
    redraw = true;
}

void mainview() {

    //Temperature block
    if (heating) {
        u8g.drawStr(0, 32, "heating on");
    }
    if (!heating) {
        u8g.drawStr(0, 32, "heating off");
    }
    switch (menu) {
      case 0:
        u8g.drawStr(0, 10, "Temperature:");
        u8g.setPrintPos(2, 22);
        u8g.print(TEMP1);
        break;
      case 1:
        u8g.drawStr(0, 10, "Ref Temperature:");      
        u8g.setPrintPos(2, 22);
        u8g.print(refTEMP);
        break;
    case 2:
        u8g.drawStr(0, 10, "Hysterisis:");
        u8g.setPrintPos(2, 22);
        u8g.print(hystTEMP);
       break;
    }
}

float termistor(float Vo){
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  return T;
}
void CalculateSensors(){

  if(readcount > 2){
     TEMP1 = TEMP1SUM / readcount;
     readcount = 0;
     TEMP1SUM = 0;
  }
 
}

void ReadSensors(){
  float T1 = termistor(analogRead(ThermistorPin));
  TEMP1SUM += T1;
  readcount += 1;
  //delay(10);
}

// the loop function runs over and over again until power down or reset
void loop() {
    if(millis() >= time_now + readdelay){
      time_now += readdelay;
      ReadSensors();
    }
    if (readcount >= 10) {
        CalculateSensors();
        temp_regulator();
        redraw = true;
    }
    if (redraw) {
        u8g.firstPage();
        do {
            mainview();
        } while (u8g.nextPage());
        redraw = false;
    } 
  enc.tick();
  if (enc.isClick()) {
        if(readcount > 1){
          CalculateSensors();
        }
        redraw = true;
        menu++;
    if (menu > 2) {
      menu = 0;
      StoreParams();
    }   
  }

  if (enc.isRight()) {
    if (menu == 1) {
      refTEMP += STEPCHANGE;
    }
    if (menu == 2) {
      hystTEMP += STEPCHANGE;
    }
    redraw = true;
  }

  if (enc.isLeft()) {
    if (menu == 1) {
    refTEMP -= STEPCHANGE;
    }
    if (menu == 2) {
      hystTEMP -= STEPCHANGE;
    }
    redraw = true;
  }
}
