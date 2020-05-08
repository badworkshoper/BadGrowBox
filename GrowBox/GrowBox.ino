/*
 Name:		GrowBox.ino
 Created:	07.05.2020 22:29:01
 Author:	badworkshoper
*/

// the setup function runs once when you press reset or power the board

// ����������
#include <Wire.h>
#include <GyverEncoder.h>
#include <U8glib.h>
#include <GyverRelay.h>
#include <HTU21D.h>

//���������
#define CLK 7			//��������� ����� ��������
#define DT 8
#define SW 9

#define pinRELE1 1			//���� ����
#define pinRELE2 2			//���� ����
#define pinRELE3 3			//���� ����

#define pinTEMP1 6			//��� ������� �����������1
#define pinTEMP2 7			//��� ������� �����������2
#define pinHUM 8			//��� ������� ���������

#define SMALLSTEP 0.1	//��� �������� ��� ���������� ��������
#define BIGSTEP 1		//��� �������� ��� �������� ��������

//
GyverRelay regulator_temp(REVERSE);
GyverRelay regulator_hum(REVERSE);
Encoder enc1(CLK, DT, SW);
HTU21D HUMP_TEMP;

float refTEMP = 24.0;	// ����������� �����������
float refHUM = 90.0;	// ����������� ���������

float hystTEMP = 2.0;	// ���������� �����������
float hystHUM = 5.0;	// ���������� ���������

float fbTEMP = 0.5;		// ���� �������� ����� �� �����������
float fbHUM = 0.5;		// ���� �������� ����� �� ���������

float TEMP_1 = 0;
float TEMP_2 = 0;
float HUM = 0;
void setup() {

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
