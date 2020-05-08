/*
 Name:		GrowBox.ino
 Created:	07.05.2020 22:29:01
 Author:	badworkshoper
*/

// the setup function runs once when you press reset or power the board

// библиотеки
#include <Wire.h>
#include <GyverEncoder.h>
#include <U8glib.h>
#include <GyverRelay.h>
#include <HTU21D.h>

//Установки
#define CLK 7			//Установка пинов энкодера
#define DT 8
#define SW 9

#define pinRELE1 1			//пины реле
#define pinRELE2 2			//пины реле
#define pinRELE3 3			//пины реле

#define pinTEMP1 6			//пин датчика температуры1
#define pinTEMP2 7			//пин датчика температуры2
#define pinHUM 8			//пин датчика влажности

#define SMALLSTEP 0.1	//шаг энкодера для медленного вращения
#define BIGSTEP 1		//шаг энкодера для быстрого вращения

//
GyverRelay regulator_temp(REVERSE);
GyverRelay regulator_hum(REVERSE);
Encoder enc1(CLK, DT, SW);
HTU21D HUMP_TEMP;

float refTEMP = 24.0;	// референсная температура
float refHUM = 90.0;	// референсная влажность

float hystTEMP = 2.0;	// гистерезис температуры
float hystHUM = 5.0;	// гистерезис влажность

float fbTEMP = 0.5;		// коэф обратной связи по температуре
float fbHUM = 0.5;		// коэф обратной связи по влажности

float TEMP_1 = 0;
float TEMP_2 = 0;
float HUM = 0;
void setup() {

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
