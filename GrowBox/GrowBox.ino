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
//#include <GyverRelay.h>
#include <HTU21D.h>
#include <TimeLib.h>
#include <DS1307RTC.h>


//some defines
#define CLK 11			//encoder button
#define DT 10
#define SW 9
#define NUM_READINGS 10		//number of readings for avarage

#define heatRELE 12			//pin for heating relay
#define coolRELE 13			//pin for colling relay
#define wetRELE 8           //pin for wetting relay
#define pinRELE3 3			//pin rele 3

#define SMALLSTEP 0.1	//step changing slow
#define BIGSTEP 1		//step changing big

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 
//
//GyverRelay regulator_temp(REVERSE);
//GyverRelay regulator_hum(REVERSE);
Encoder enc(CLK, DT, SW);
HTU21D HTSensor;


float refHUM = 90.0;	// reference humiliation value
float refTEMP = 24.5;	// reference temperature value

float hystTEMP = 3;	// hysteresis for temperature
float hystHUM = 5.0;	// hysteresis for humiliation

float fbTEMP = 0.5;		// koef os for temperature 
float fbHUM = 0.5;		// koef os for humiliation

//float TEMP_1 = 0;
float TEMP_2 = 23.5;
//float HUM1 = 0;
float HUM2 = 90;
bool heating = false;
bool cooling = false;

bool wetting = false;
bool drying = false;
tmElements_t tm;



void setup() {
    u8g.setFont(u8g_font_unifontr);
	HTSensor.begin();
    pinMode(heatRELE, OUTPUT);
    pinMode(coolRELE, OUTPUT);
    digitalWrite(heatRELE, LOW);
    digitalWrite(coolRELE, LOW);

    
}
//Temperature regulating
void temp_regulator() {
    if (TEMP_2 <= refTEMP - hystTEMP * 0.5) {
        heating = true;
        cooling = false;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
    if (TEMP_2 >= refTEMP + hystTEMP * 0.5) {
        heating = false;
        cooling = true;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
    if (TEMP_2 <= refTEMP + hystTEMP * 0.5 && TEMP_2  >= refTEMP - hystTEMP * 0.5) {
        heating = false;
        cooling = false;
        digitalWrite(heatRELE, !heating);
        digitalWrite(coolRELE, !cooling);
    }
   
}
//Humiliation regulating
void hum_regulator() {
    if (HUM2 <= refHUM - hystHUM * 0.5) {
        wetting = true;
        drying = false;
    }
    if (HUM2 >= refHUM + hystHUM * 0.5) {
        wetting = false;
        drying = true;
    }
    if (HUM2 <= refHUM + hystHUM * 0.5 && HUM2 >= refHUM - hystHUM * 0.5) {
        wetting = false;
        drying = false;
    }
}




void mainview() {
;
    //Temperature block
    u8g.drawStr(0, 10, "TEMP");
    u8g.setPrintPos(2, 24);
    u8g.print(refTEMP);
    u8g.drawFrame(0, 12, 50, 14); // frame ref value
    u8g.setPrintPos(2, 38);
    u8g.print(TEMP_2);
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

    //time block
    if (RTC.read(tm)) {
        u8g.setPrintPos(43, 64);
        u8g.print(tm.Hour);
        u8g.setPrintPos(58, 64);
        u8g.print(":");
        u8g.setPrintPos(64, 64);
        u8g.print(tm.Minute);

    }
    
    //Humidity block
    u8g.drawStr(66, 10, "HUM");
    u8g.setPrintPos(68, 24);
    u8g.print(refHUM);
    u8g.drawFrame(66, 12, 50, 14); // frame ref value
    u8g.setPrintPos(68, 38);
    u8g.print(HUM2);
    if (drying) {
        u8g.drawStr(66, 50, "drying");
    }

    if (wetting) {
        u8g.drawStr(66, 50, "wetting");
    }
    if (wetting == false && drying == false) {
        u8g.drawStr(66, 50, "OK");
    }
}

// the loop function runs over and over again until power down or reset
void loop() {
    u8g.firstPage();
    do {

        mainview();
    } while (u8g.nextPage());
	float sum_temp = 0;
	float sum_hum = 0;
	for (int i = 0; i < NUM_READINGS; i++) {
		sum_temp += HTSensor.readTemperature();
		sum_hum += HTSensor.readHumidity();
	}
    TEMP_2 = sum_temp / NUM_READINGS;
	HUM2 = sum_hum / NUM_READINGS;
    temp_regulator();
    hum_regulator();
    // rebuild the picture after some delay
    delay(100);
  
}
