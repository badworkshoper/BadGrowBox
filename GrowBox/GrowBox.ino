/*
 Name:		GrowBox.ino
 Created:	07.05.2020 22:29:01
 Author:	badworkshoper
*/

// the setup function runs once when you press reset or power the board

// ����������
#include <DHT.h>
#include <Wire.h>
#include <GyverEncoder.h>
#include <U8glib.h>
#include <GyverRelay.h>
#include <HTU21D.h>

//���������
#define CLK 11			//encoder button
#define DT 10
#define SW 9

#define pinRELE1 1			//pin rele 1
#define pinRELE2 2			//pin rele 2
#define pinRELE3 3			//pin rele 3

#define pinTEMP1 7			//pin temperature1 sensor
#//define pinTEMP2 7			//pin temperature2 sensor
//#define pinHUM 8			//pin temperature3 sensor

#define SMALLSTEP 0.1	//step changing slow
#define BIGSTEP 1		//step changing big
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 
//
GyverRelay regulator_temp(REVERSE);
GyverRelay regulator_hum(REVERSE);
Encoder enc1(CLK, DT, SW);
HTU21D HUMP_TEMP;
DHT dht(pinTEMP1, DHT11);


float refHUM = 90.0;	// reference humiliation value
float refTEMP = 23.5;	// reference temperature value

float hystTEMP = 3.0;	// hysteresis for temperature
float hystHUM = 5.0;	// hysteresis for humiliation

float fbTEMP = 0.5;		// koef os for temperature 
float fbHUM = 0.5;		// koef os for humiliation

float TEMP_1 = 0;
float TEMP_2 = 0;
float HUM1 = 0;
float HUM2 = 0;

bool heating = false;
bool cooling = false;

bool wetting = false;
bool drying = false;




void setup() {
    u8g.setFont(u8g_font_unifontr);
    dht.begin();
}
void temp_regulator() {
    if (TEMP_2 <= refTEMP - hystTEMP/2) {
        heating = true;
        cooling = false;
    }
    if (TEMP_2 >= refTEMP + hystTEMP / 2) {
        heating = false;
        cooling = true;
    }
    if (TEMP_2 <= refTEMP + hystTEMP/2 && TEMP_2  >= refTEMP - hystTEMP / 2) {
        heating = false;
        cooling = false;
    }
}
void hum_regulator() {
    if (HUM2 <= refHUM - hystHUM / 2) {
        wetting = true;
        drying = false;
    }
    if (HUM2 >= refHUM + hystHUM / 2) {
        wetting = false;
        drying = true;
    }
    if (HUM2 <= refHUM + hystHUM / 2 && HUM2 >= refHUM - hystHUM / 2) {
        wetting = false;
        drying = false;
    }
}




void mainview() {

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


    u8g.drawVLine(62, 0, 64); //separating


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
    TEMP_1 = dht.readTemperature();
    HUM1 = dht.readHumidity();
    TEMP_2 = HUMP_TEMP.readTemperature();
    HUM2 = HUMP_TEMP.readHumidity();
    temp_regulator();
    hum_regulator();
    // rebuild the picture after some delay
    delay(500);
  
}
