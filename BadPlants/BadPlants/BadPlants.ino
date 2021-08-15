#include <GyverEncoder.h>
#include <Wire.h>
#include <U8glib.h>


#define SW 8
#define DT 9
#define CLK 10
#define PumpPin 11

uint32_t watering_delay = 43200000; //12h
long timer = 0;
long watering_timer = 0;
int watering_time = 5000;
bool watering = false;

uint8_t menu = 0;
bool redraw = false;

U8GLIB_SSD1306_128X64 display(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);


void setup() {

  display.setFont(u8g_font_unifontr);
  enc.setType(TYPE2);
  pinMode(PumpPin, OUTPUT);
  Serial.begin(9600);
}
void DisplayUpdate(){
    display.drawStr(0, 10, "W.delay");
    display.setPrintPos(75, 10);
    display.print(watering_delay/3600000);

    display.drawStr(0, 24, "W.time");
    display.setPrintPos(75, 24);
    display.print(watering_time/1000); 

    display.drawStr(0, 38, "Pump");
    if(watering){
      display.drawStr(50, 38, "On");
    }
    else{
      display.drawStr(50, 38, "Off");
    }
    
  switch(menu){
    case 0:
      break;
    case 1:
      display.drawHLine(0, 11, 50);
      break;
    case 2:
      display.drawHLine(0, 25, 50);
      break;
     case 3:
      display.drawHLine(0, 39, 50);
      break;
  }
}



void pump(bool state){
  watering = state;
  switch(state){
    case true:
      watering_timer = millis();
      Serial.println("Pump On");
      break;
    case false:
      timer = millis();
      Serial.println("Pump Off");
      break; 
  }
  redraw = true;      
  digitalWrite(PumpPin, watering);
}

void loop() 
{
  if(!watering && millis() - timer >= watering_delay){
    pump(true);
  }
  enc.tick();
  if(watering && millis() - watering_timer >= watering_time ){
    pump(false);  
  }
  if (redraw) {
        display.firstPage();
        do {
            DisplayUpdate();
        } while (display.nextPage());
        redraw = false;
    }

  if (enc.isClick()) {
    if(menu < 3){
      menu++;
    }
    else{
      menu = 0;
    }
    redraw = true;
  }

  
  if(enc.isHolded()){
    pump(true);
    redraw = true;
  }
  
  if(enc.isRight()){
    switch (menu){
      case 0:  
        break;
      case 1:
        watering_delay += 3600000;
        break;
      case 2:
        watering_time += 1000;
        break;
     }
     redraw = true; 
   }

  if(enc.isLeft()){
    switch (menu){
      case 0: 
        break;
      case 1:
        watering_delay -= 3600000;
        break;
      case 2:
        watering_time = watering_time < 0 ? 1000 : watering_time - 1000;
        break;
     }
     redraw = true; 
   }
}
