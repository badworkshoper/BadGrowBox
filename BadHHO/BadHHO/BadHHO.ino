/*
 Name:    BadHHO.ino
 Created: 07.05.2020 22:29:01
 Author:  badworkshoper
*/

// libraries
#include <Wire.h>
#include <U8glib.h>
#include <GyverPWM.h>
#include <GyverEncoder.h>
//#include <EEPROM.h>

//encoder
#define SW 4
#define DT 5
#define CLK 6
#define PressureSensor A3
#define DutyPin A2
#define LedPin 7
#define PWMPin 11

//Light block
int8_t HUE = 0;
int8_t Bright = 0;


//pressure block
float pressure = 0;
float pressure_cutoff = 21;
bool pressure_overload = false;
float pressure_coef[] = {1.0, 0.0689476, 0.06804599, 6.894757}; //psi, bar, atm, kPa

int pressure_unit = 0; //0 - psi, 1 - bar, 2 - atm, 3 - kPa
char* pressure_units[] = {"psi", "bar", "atm", "kPa"};
char* menu_items[] = {"Pressure", "Duty", "Cutoff", "P.Unit", "HUE", "Bright"};

//time, timers and read block
int16_t read_delay = 100;
int16_t duty_delay = 100;
unsigned long time_now = 0;
int read_count = 10;

//Duty block
int duty_percent = 0;
int duty = 0;

//Display block
uint8_t menu = 0;
uint8_t page = 0;
bool redraw = false;

uint8_t step_change = 1;
U8GLIB_SSD1306_128X64 display(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);

void setup() {
  display.setFont(u8g_font_unifontr);
  enc.setType(TYPE2);
  pinMode(PressureSensor, INPUT);
  pinMode(DutyPin, INPUT);
  pinMode(PWMPin, OUTPUT);
  PWM_prescaler(PWMPin, 1);
  Serial.begin(9600);
}


void ReadPressure() {
  static float pressure_SUM = 0;
  static int i = 0;
  if(i >= read_count){
    pressure = map(pressure_SUM / (float)i, 0, 1023, 0, 200) * pressure_coef[pressure_unit];
    //pressure *= 0.068947572931783;
    //pressure = pressure_SUM / (float)i;
    i = 0;
    pressure_SUM = 0;
    Serial.println(pressure);
    pressure_overload = pressure * pressure_coef[pressure_unit] < pressure_cutoff * pressure_coef[pressure_unit];
    redraw = true;
  }
  i += 1;
  pressure_SUM += analogRead(PressureSensor);
}

void Duty(){
 static long timer = 0;
 static int duty_read = 0;

 if(millis() - timer >= duty_delay){
   timer = millis();
    duty_read = analogRead(DutyPin);
    duty_read = duty_read >= 110 ? duty_read : 0;
    duty = map(duty_read, 0, 1023, 0, 255);
    duty_percent = map(duty, 0, 255, 0, 100);
 }
}

void DisplayUpdate(){
  if(page == 0){
    display.drawStr(0, 10, menu_items[0]);
    display.setPrintPos(75, 10);
    display.print(pressure);

    display.drawStr(0, 24, menu_items[2]);
    display.setPrintPos(75, 24);
    display.print(pressure_cutoff * pressure_coef[pressure_unit]); 

    display.drawStr(0, 38, menu_items[3]);
    display.drawStr(75, 38, pressure_units[pressure_unit]);

    display.drawStr(0, 52, menu_items[1]);
    display.setPrintPos(75, 52);
    display.print(duty_percent);


  }

  if(page == 1){
    display.drawStr(0, 10, "RGB");
    display.drawStr(0, 24, menu_items[4]);
    display.setPrintPos(75, 24);
    display.print(HUE);
    display.drawStr(0, 38, menu_items[5]);
    display.setPrintPos(75, 38);
    display.print(Bright);
  }
  switch(menu){
    case 0:
    
      break;
    case 1:
      display.drawHLine(0, 25, 50);
      break;
    case 2:
      display.drawHLine(0, 39, 50);
      break;
  }
}


// the loop function runs over and over again until power down or reset
void loop() {
    if(millis() - time_now >= read_delay){
      time_now = millis();
      ReadPressure();
    }
  
    Duty();
    analogWrite(PWMPin, duty * pressure_overload);
    
    if (redraw) {
        display.firstPage();
        do {
            DisplayUpdate();
        } while (display.nextPage());
        redraw = false;
    }
  enc.tick();
  if (enc.isClick()) {
    if(menu < 2){
      menu++;
    }
    else{
      menu = 0;
    }
    redraw = true;
  }
  if(enc.isRight()){
    if(page == 0){
      switch (menu){
        case 0:
          page += 1;
          Serial.print("Page - ");
          Serial.println(page);   
          break;
        case 1:
          pressure_cutoff += step_change * 0.1;
          break;
        case 2:
          pressure_unit = pressure_unit == 3? 0 : pressure_unit + 1;
          break;
      } 
    }
    else {
      switch (menu){
        case 0:
          page = 0;
          Serial.print("Page - ");
          Serial.println(page);   
          break;
        case 1:
          HUE = HUE == 254? 254 : HUE + step_change * 2;
          break;
        case 2:
          Bright = Bright == 100 ? 100 : Bright + step_change * 2;
          break;
      }
    }
    redraw = true;
  }
  if(enc.isLeft()){
    if(page == 0){
      switch (menu){
        case 0:
          page += 1;
          Serial.print("Page - ");
          Serial.println(page);   
          break;
        case 1:
          pressure_cutoff = pressure_cutoff == 0 ? pressure_cutoff : pressure_cutoff - step_change * 0.1;
          break;
        case 2:
          pressure_unit = pressure_unit == 0 ? 3 : pressure_unit - 1;
          break;
      } 
    }
    else{
      switch (menu){
        case 0:
          page = 0;
          Serial.print("Page - ");
          Serial.println(page);          
          break;
        case 1:
          HUE = HUE == 0? 0 : HUE - step_change * 2;
          break;
        case 2:
          Bright = Bright == 0 ? 0 : Bright - step_change * 2;
          break;
      }
    }
    redraw = true;
  }  
}
