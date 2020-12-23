/*
 Name:		GrowBox.ino
 Created:	07.05.2020 22:29:01
 Author:	badworkshoper
*/

// the setup function runs once when you press reset or power the board

// libraries
#include <Wire.h>
#include <GyverEncoder.h>
#include <U8glib.h>
#include <HTU21D.h>
#include <TimeLib.h>
#include <DS1307RTC.h>


//some defines
#define SW 9
#define DT 10
#define CLK 11

#define NUM_READINGS 10		//number of readings for avarage

#define heatRELE 12			//pin for heating relay
#define coolRELE 13			//pin for colling relay
#define wetRELE 8           //pin for wetting relay
#define pinRELE3 3			//pin rele 3


#define STEPCHANGE 10	//step changing slow


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 

Encoder enc(CLK, DT, SW);
HTU21D HTSensor;


int period = 10000;
unsigned long time_now = 0;
//Termistor block
int ThermistorPin = A0;
float R1 = 10110;
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;
//Termistor  block

int refHUM = 9000;	// reference humiliation value
int refTEMP = 2450;	// reference temperature value

int hystTEMP = 30;	// hysteresis for temperature
int hystHUM = 100;	// hysteresis for humiliation


int TEMP1SUM = 0;
float TEMP2SUM = 0;
int HUM1SUM = 0;
int TEMP1 = 2350;
float TEMP2 = 0;
int HUM1 = 9000;

bool heating = false;
bool cooling = false;

bool wetting = false;
bool drying = false;
tmElements_t tm;

int menu = 0;
int i = 0;

bool redraw = false;


void setup() {
	//enc.setTickMode(AUTO);
    enc.setType(TYPE2);
    //delay(100);
    TEMP1 = HTSensor.readTemperature() * 100;
    HUM1 = HTSensor.readHumidity() * 100;
	Serial.begin(9600);
    u8g.setFont(u8g_font_unifontr);
	HTSensor.begin();
    pinMode(heatRELE, OUTPUT);
    pinMode(coolRELE, OUTPUT);
    digitalWrite(heatRELE, LOW);
    digitalWrite(coolRELE, LOW);
    
}



//Temperature regulating
void temp_regulator() {
   
    if (TEMP1 <= refTEMP - hystTEMP * 0.5) {
        heating = true;
        cooling = false;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
    if (TEMP1 >= refTEMP + hystTEMP * 0.5) {
        heating = false;
        cooling = true;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
    if (TEMP1 <= refTEMP + hystTEMP * 0.5 && TEMP1  >= refTEMP - hystTEMP * 0.5) {
        heating = false;
        cooling = false;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
   
    redraw = true;
}
//Humiliation regulating
void hum_regulator() {
    if (HUM1 <= refHUM - hystHUM * 0.5) {
        wetting = true;
        drying = false;
    }
    if (HUM1 >= refHUM + hystHUM * 0.5) {
        wetting = false;
        drying = true;
    }
    if (HUM1 <= refHUM + hystHUM * 0.5 && HUM1 >= refHUM - hystHUM * 0.5) {
        wetting = false;
        drying = false;
    }
}
float termistor(float Vo){
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  return T;
}
void mainview() {

    //Temperature block
    u8g.drawStr(0, 10, "TEMP");
    u8g.setPrintPos(2, 24);
    u8g.print(refTEMP * 0.01);
    u8g.drawFrame(0, 12, 50, 14); // frame ref value
    u8g.setPrintPos(2, 38);
    u8g.print(TEMP1 * 0.01);
    if (cooling) {
        u8g.drawStr(0, 50, "cooling");
    }

    if (heating) {
        u8g.drawStr(0, 50, "heating");
    }
    if (heating == false && cooling == false) {
        u8g.drawStr(0, 50, "OK");
    }


    u8g.drawVLine(62, 0, 50); //separating
    u8g.setPrintPos(43, 64);
    u8g.print(TEMP2);
    //time block
    /*
    if (RTC.read(tm)) {
        u8g.setPrintPos(43, 64);
        u8g.print(tm.Hour);
        u8g.setPrintPos(58, 64);
        u8g.print(":");
        u8g.setPrintPos(64, 64);
        u8g.print(tm.Minute);

    }
    */
    //u8g.setPrintPos(43, 64);
    //u8g.print(timetovoid);
    
    //Humidity block
    u8g.drawStr(66, 10, "HUM");
    u8g.setPrintPos(68, 24);
    u8g.print(refHUM * 0.01);
    u8g.drawFrame(66, 12, 50, 14); // frame ref value
    u8g.setPrintPos(68, 38);
    u8g.print(HUM1 * 0.01);
    if (drying) {
        u8g.drawStr(66, 50, "drying");
    }

    if (wetting) {
        u8g.drawStr(66, 50, "wetting");
    }
    if (wetting == false && drying == false) {
        u8g.drawStr(66, 50, "OK");
    }

	switch (menu) {
	case 0:
		break;
	case 1:
		u8g.setPrintPos(50, 38);
		u8g.print("*");
		break;
	case 2:
		u8g.setPrintPos(120, 38);
		u8g.print("*");
		break;
	}
}

void filter() {

}

// the loop function runs over and over again until power down or reset
void loop() {
    TEMP1SUM = 0;
    HUM1SUM = 0;
    TEMP2SUM = 0;
    if (millis() >= time_now + period) {
        time_now += period;

        for(int i = 0; i < NUM_READINGS; i++){
          TEMP1SUM += int(HTSensor.readTemperature() * 100);
          HUM1SUM += int(HTSensor.readHumidity() * 100);
          TEMP2SUM += termistor(analogRead(ThermistorPin));
          delay(10);
        }
        TEMP1 = TEMP1SUM / NUM_READINGS;
        HUM1 = HUM1SUM / NUM_READINGS;
        TEMP2 = TEMP2SUM / NUM_READINGS;
        //filter();
        temp_regulator();
        hum_regulator();
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
        redraw = true;
        menu++;
		if (menu > 2) {
			menu = 0;
		}
        
	}

	if (enc.isRight()) {
		if (menu == 1) {
			refTEMP += STEPCHANGE;
		}
		if (menu == 2) {
			refHUM += STEPCHANGE;
		}
	    Serial.println("Right");
        redraw = true;
	}

	if (enc.isLeft()) {
		if (menu == 1) {
		refTEMP -= STEPCHANGE;

		}
		if (menu == 2) {
			refHUM -= STEPCHANGE;

		}
		//Serial.println("Left");
        redraw = true;
	}

}
