#define SW 4
#define DT 3
#define CLK 2
#define LedPin 12
#define DIR 6
#define STEP 7
#define BUZZER 13
#define MC1 10
#define MC2 9
#define MC3 8
#define wash_adress 2
#define flash_adress 7

//#include "GyverStepper2.h"
#include <Wire.h>
#include <U8glib.h>
#include <GyverEncoder.h>
#include <avr/eeprom.h>

#define wash_adress 10
#define flash_adress 30
bool redraw = false;
uint8_t page = 0;
uint8_t parameter = 0;
bool changeParamMode = false;
bool paramSelect = false;

struct Settings {
  int speed;
  uint32_t  time;
  boolean light;
};
Settings Wash;
Settings Flash;


U8GLIB_SSD1306_128X64 display(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);
//GStepper2<STEPPER2WIRE> stepper(200, STEP, DIR);

void DisplaySettings(struct Settings *params, char *type){
    display.drawStr(20, 10, type);
    display.drawStr(62, 10, "mode");
    display.drawStr(0, 24, "Speed:");
    display.setPrintPos(90, 24);
    display.print(params->speed); 
    display.drawStr(0, 38, type);
    display.drawStr(30, 38, "  time:");
    display.setPrintPos(90, 38);
    display.print(params->time);
    display.drawStr(0, 52, "Use UV:");
    display.setPrintPos(90, 52);
    display.print(params->light);
    display.drawStr(42, 64, "START");
    display.drawHLine(0, 11+13*parameter, 128);

    if(paramSelect){
      display.drawStr(120, 11+14*parameter, "*");
    }
    if(changeParamMode){
      display.drawStr(120, 10, "*");
    }
}
void DisplayMain(){
  display.drawStr(2, 18, "<-Choose mode->");
  display.drawStr(20, 32, "Time to end: ");
  display.drawStr(32, 48, "--:--:--");
}

void ChangeParam(struct Settings *settings, int change){
  switch (parameter){
    case 1:
        settings->speed+=change;
        //Serial.print("Speed Setting --> ");
        //Serial.println(settings->speed);
      break;
    case 2:
      settings->time+=change*10000;
      //Serial.print("Time Setting --> ");
      //Serial.println(settings->time);
      break;
    case 3:
      if(change > 0){
        settings->light = true;
        //Serial.print("Light Setting --> ");
        //Serial.println(settings->light);
      }
      else{
        settings->light = false;
        //Serial.print("Light Setting --> ");
        //Serial.println(settings->light);

      }
      break;
  }
}


void setup() {

  
  Wash.speed = 100;
  Wash.time = 100000;
  Wash.light = false;
  Serial.begin(9600);
  Flash.speed = 10;
  Flash.time = 10000;
  Flash.light = true;

  enc.setType(TYPE2);
  pinMode(LedPin, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 
  pinMode(MC1, OUTPUT); 
  pinMode(MC2, OUTPUT); 
  pinMode(MC3, OUTPUT);  
  display.setFont(u8g_font_unifontr);
  //stepper.setAcceleration(200);

  int testvalue = 0;
  testvalue = eeprom_read_byte(0);
  if(testvalue == -1){
    StoreParams(&Flash, flash_adress);
    StoreParams(&Wash, wash_adress);
  }
  eeprom_read_block((void*)&Wash, wash_adress, sizeof(Wash));
  eeprom_read_block((void*)&Flash, flash_adress, sizeof(Flash));
  redraw = true;
}


void StoreParams(struct Settings *settings, uint8_t adress){
  eeprom_write_block((void*)&settings, adress, sizeof(settings));
}
void turnHandler(bool side){
  //side true - right
  //side false - left
  //Serial.println("Call turnHandler");
  if(parameter == 0 && changeParamMode == false){
    if(side){
      //Serial.print("Page right --> ");
      page = page == 2? 0: page+1;
    }
    else{
      //Serial.print("Page left --> ");
      page = page == 0? 2: page-1;
    }
  }
  if(changeParamMode == true && paramSelect == false){
    if(side){
      //Serial.println("Change parameter +1");
      parameter = parameter == 4? 0: parameter+1;
    }
    else{
      //Serial.println("Change parameter -1");
      parameter = parameter == 0? 4: parameter-1;
    }
  }
  else if(changeParamMode == true && paramSelect == true){
    if(page == 1){
      // Serial.println("Change parameter in settings +1");
      ChangeParam(&Flash, side? 10:-10);
    }
    else if(page == 2){
      // Serial.println("Change parameter in settings -1");
      ChangeParam(&Wash, side? 10:-10);
    }
  }
  
  // Serial.print("page --> ");
  // Serial.println(page);
  // Serial.print("parameter --> ");
  // Serial.println(parameter);
} 
void clickHandler(){
  //Serial.println("Click");
  if(page != 0 && parameter != 0){
    paramSelect = !paramSelect;
    // Serial.println("Switch paramSelect Bool");

  }
  if(page != 0 && parameter == 0){
    changeParamMode = !changeParamMode;
    // Serial.println("Switch changeParamMode Bool");
    StoreParams(&Flash, flash_adress);
    StoreParams(&Wash, wash_adress);
  }
  // Serial.print("changeParamMode --> ");
  // Serial.println(changeParamMode);
  // Serial.print("paramSelect --> ");
  // Serial.println(paramSelect);
}

void loop() {
  enc.tick();
  //stepper.tick();
  static uint32_t tmr;
  if (millis() - tmr >= 2000) {
    tmr = millis();
    //Serial.println(stepper.pos);
    redraw = true;
  }
  if (redraw) {
    display.firstPage();
    do {
      switch (page){
        case 0:
          DisplayMain();
          break;
        case 1:
          DisplaySettings(&Flash, "Flash");
          break;
        case 2:
          DisplaySettings(&Wash, "Wash");
          break;
      }
    } while (display.nextPage());
    redraw = false;
    }
  if(enc.isLeft()){
    turnHandler(false);
    redraw = true;
  }
  if(enc.isRight()){
    turnHandler(true);
    redraw = true;
  } 
  if(enc.isClick()){
    clickHandler();
    redraw = true;
  }
}
