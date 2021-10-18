#define SW 2
#define DT 3
#define CLK 4
#define LedPin 12
#define DIR 6
#define STEP 7
#define BUZZER 13
#define MC1 10
#define MC2 9
#define MC3 8


#include "GyverStepper2.h"
#include <Wire.h>
#include <U8glib.h>
#include <GyverEncoder.h>
#include <EEPROM.h>

U8GLIB_SSD1306_128X32 display(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);
GStepper2<STEPPER2WIRE> stepper(200, STEP, DIR);



void setup() {
  // put your setup code here, to run once:
  enc.setType(TYPE2);
  pinMode(LedPin, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(MC1, OUTPUT); 
  pinMode(MC2, OUTPUT); 
  pinMode(MC3, OUTPUT);  
  display.setFont(u8g_font_unifontr);
  stepper.setRunMode(KEEP_SPEED)
}

void loop() {
  enc.tick();
  stepper.tick();
  static uint32_t tmr;
  if (millis() - tmr >= 30) {
    tmr = millis();
    Serial.println(stepper.pos);
    stepper.setSpeedDeg(100);
  }
}
