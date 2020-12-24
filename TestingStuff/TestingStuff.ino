/*
 Name:    GrowBox.ino
 Created: 07.05.2020 22:29:01
 Author:  badworkshoper
*/

// the setup function runs once when you press reset or power the board

// libraries
#include <Wire.h>
#include <U8glib.h>
//some defines
#define NUM_READINGS 10   //number of readings for avarage


U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 


int period = 1000;
unsigned long time_now = 0;
float TEMP1SUM = 0;
float TEMP1 = 2350;
int i = 0;
bool redraw = false;

int ThermistorPin = A0;
int Vo;
float R1 = 10000; // value of R1 on board
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;

void setup() {

  Serial.begin(9600);
    u8g.setFont(u8g_font_unifontr);
}


void mainview() {

    //Temperature block
    u8g.drawStr(0, 10, "TEMP");
    u8g.setPrintPos(2, 24);
    u8g.print(TEMP1);
}

float termistor(float Vo){
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  return T;
}
// the loop function runs over and over again until power down or reset
void loop() {
    TEMP1SUM = 0;
    if (millis() >= time_now + period) {
        time_now += period;
        for(int i = 0; i < NUM_READINGS; i++){
          TEMP1SUM += termistor(analogRead(ThermistorPin));
          delay(10);
        }
        TEMP1 = TEMP1SUM / NUM_READINGS;
        redraw = true;
    }
    if (redraw) {
        u8g.firstPage();
        do {
            mainview();
        } while (u8g.nextPage());
        redraw = false;
    }


}
