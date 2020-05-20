/*
 Name:		GrowBox.ino
 Created:	07.05.2020 22:29:01
 Author:	badworkshoper
*/

// the setup function runs once when you press reset or power the board

// libraries
//#include <encMinim.h>
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

#define SMALLSTEP 10	//step changing slow
#define BIGSTEP 1		//step changing big


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 

Encoder enc(CLK, DT, SW);
HTU21D HTSensor;

int period = 40;
unsigned long time_now = 0;


float refHUM = 90.0;	// reference humiliation value
float refTEMP = 24.5;	// reference temperature value

float hystTEMP = 3;	// hysteresis for temperature
float hystHUM = 5.0;	// hysteresis for humiliation

//float TEMP_1 = 0;
float TEMP_2 = 23.5;
//float HUM1 = 0;
float HUM2 = 90;
float TEMPArray[NUM_READINGS];
float HUMArray[NUM_READINGS];

bool heating = false;
bool cooling = false;

bool wetting = false;
bool drying = false;
tmElements_t tm;
int menu = 0;
int i = 0;

float timetovoid = 0;

void setup() {
	//enc.setTickMode(AUTO);
    enc.setType(TYPE2);
    //delay(100);
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
    //u8g.setPrintPos(43, 64);
    //u8g.print(timetovoid);
    
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
float arraySUM(float array[]) {
    float sum = 0;
    for (int a = 0; a <= NUM_READINGS - 1; a++) {
        sum += array[a];
    }
    sum = sum / NUM_READINGS;
    return sum;
}

void filtering() {
    TEMPArray[i] = HTSensor.readTemperature();
    HUMArray[i] = HTSensor.readHumidity();
    float temp = 0;
    float hum = 0;
    switch (i) {
    case NUM_READINGS:
        TEMP_2 = arraySUM(TEMPArray);
        HUM2 = arraySUM(HUMArray);
        i = -1;
        break;
    default:
        break;
    }
    i++;
}

// the loop function runs over and over again until power down or reset
void loop() {
    if (millis() >= time_now + period) {
        time_now += period;
    }
	enc.tick();
	if (enc.isClick()) {
		menu++;
		if (menu > 2) {
			menu = 0;
		}
	}

    u8g.firstPage();
	do {
        mainview();
    } while (u8g.nextPage());

	if (enc.isRight()) {
		if (menu == 1) {
			refTEMP += SMALLSTEP * 0.01;
		}
		if (menu == 2) {
			refHUM += SMALLSTEP * 0.01;
		}
	    Serial.println("Right");
	}

	if (enc.isLeft()) {
		if (menu == 1) {
		refTEMP -= SMALLSTEP *0.01;
		}
		if (menu == 2) {
			refHUM -= SMALLSTEP * 0.01;
		}
		Serial.println("Left");
	}
    //filtering();
    //temp_regulator();
    //hum_regulator();
}
