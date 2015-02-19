/*************************************************************************
 * File Name          : Firmware.ino
 * Author             : Ander
 * Updated            : Ander
 * Version            : V1.10101
 * Date               : 03/06/2014
 * Description        : Firmware for Makeblock Electronic modules with Scratch.
 * License            : CC-BY-SA 3.0
 * Copyright (C) 2013 - 2014 Maker Works Technology Co., Ltd. All right reserved.
 * http://www.makeblock.cc/
 **************************************************************************/
#include "MePort.h"
#include "MeDCMotor.h" 

#define NB_MOTOR 4
MeDCMotor dc[NB_MOTOR];

typedef struct MeModule {
	int device;
	int port;
	int slot;
	int pin;
	int index;
	float values[3];
} MeModule;

union {
	byte byteVal[4];
	float floatVal;
	long longVal;
} val;

union {
	byte byteVal[8];
	double doubleVal;
} valDouble;

union {
	byte byteVal[2];
	short shortVal;
} valShort;
MeModule modules[12];

int analogs[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };
String mVersion = "1.1.102";
boolean isAvailable = false;

int len = 52;
char buffer[52];
char bufferBt[52];
byte index = 0;
byte dataLen;
byte modulesLen = 0;
boolean isStart = false;
unsigned char irRead;
char serialRead;
#define VERSION 0
#define MOTOR 10
#define DIGITAL 30
#define ANALOG 31
#define PWM 32
#define TONE 34
#define TIMER 50

#define GET 1
#define RUN 2
#define RESET 4
#define START 5
float angleServo = 90.0;
unsigned char prevc = 0;
double lastTime = 0.0;
double currentTime = 0.0;

void setup() {
	Serial.begin(115200);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
	delay(300);
	digitalWrite(13, LOW);

	dc = {
		dc(M1), dc(M2), dc(PORT_1), dc(PORT_2)
	};
}

MeDCMotor *getDc(int port) {
	for(byte m = 0; m < NB_MOTOR; m++) {
		if (dc[m]._port == port) {
			return &dc[m];
		}
	}
	return NULL;
}

void loop() {
	currentTime = millis() / 1000.0 - lastTime;
	readSerial();
	if (isAvailable) {
		unsigned char c = serialRead & 0xff;
		if (c == 0x55 && isStart == false) {
			if (prevc == 0xff) {
				index = 1;
				isStart = true;
			}
		} else {
			prevc = c;
			if (isStart) {
				if (index == 2) {
					dataLen = c;
				} else if (index > 2) {
					dataLen--;
				}
				writeBuffer(index, c);
			}
		}
		index++;
		if (index > 51) {
			index = 0;
			isStart = false;
		}
		if (isStart && dataLen == 0 && index > 3) {
			isStart = false;
			parseData();
			index = 0;
		}
	}
	for(byte m = 0; m < NB_MOTOR; m++) {
		dc[m].step();
	}
}

unsigned char readBuffer(int index) {
	return buffer[index];
}
void writeBuffer(int index, unsigned char c) {
	buffer[index] = c;
}
void writeHead() {
	writeSerial(0xff);
	writeSerial(0x55);
}
void writeEnd() {
	Serial.println();
}
void writeSerial(unsigned char c) {
	Serial.write(c);
}
void readSerial() {
	isAvailable = false;
	if (Serial.available() > 0) {
		isAvailable = true;
		serialRead = Serial.read();
	}
}

/*
 ff 55 len idx action device port  slot  data a
 0  1  2   3   4      5      6     7     8
 */
void parseData() {
	isStart = false;
	int idx = readBuffer(3);
	int action = readBuffer(4);
	int device = readBuffer(5);
	switch (action) {
	case GET:
		writeHead();
		writeSerial(idx);
		readSensor(device);
		writeEnd();
		break;
	case RUN:
		runModule(device);
		callOK();
		break;
	case RESET:
		for(byte m = 0; m < NB_MOTOR; m++) {
			dc[m].stop();
		}
		callOK();
		break;
	case START:
		callOK();
		break;
	}
}
void callOK() {
	writeSerial(0xff);
	writeSerial(0x55);
	writeEnd();
}
void sendByte(char c) {
	writeSerial(1);
	writeSerial(c);
}
void sendString(String s) {
	int l = s.length();
	writeSerial(4);
	writeSerial(l);
	for (int i = 0; i < l; i++) {
		writeSerial(s.charAt(i));
	}
}
void sendFloat(float value) {
	writeSerial(0x2);
	val.floatVal = value;
	writeSerial(val.byteVal[0]);
	writeSerial(val.byteVal[1]);
	writeSerial(val.byteVal[2]);
	writeSerial(val.byteVal[3]);
}
void sendShort(double value) {
	writeSerial(3);
	valShort.shortVal = value;
	writeSerial(valShort.byteVal[0]);
	writeSerial(valShort.byteVal[1]);
	writeSerial(valShort.byteVal[2]);
	writeSerial(valShort.byteVal[3]);
}
void sendDouble(double value) {
	writeSerial(2);
	valDouble.doubleVal = value;
	writeSerial(valDouble.byteVal[0]);
	writeSerial(valDouble.byteVal[1]);
	writeSerial(valDouble.byteVal[2]);
	writeSerial(valDouble.byteVal[3]);
}
short readShort(int idx) {
	valShort.byteVal[0] = readBuffer(idx);
	valShort.byteVal[1] = readBuffer(idx + 1);
	return valShort.shortVal;
}
float readFloat(int idx) {
	val.byteVal[0] = readBuffer(idx);
	val.byteVal[1] = readBuffer(idx + 1);
	val.byteVal[2] = readBuffer(idx + 2);
	val.byteVal[3] = readBuffer(idx + 3);
	return val.floatVal;
}
void runModule(int device) {
	//0xff 0x55 0x6 0x0 0x1 0xa 0x9 0x0 0x0 0xa
	int port = readBuffer(6);
	int pin = port;
	switch (device) {
	case MOTOR:
		{
			valShort.byteVal[0] = readBuffer(7);
			valShort.byteVal[1] = readBuffer(8);
			MeDCMotor *dc = getDc(port);
			if (dc) {
				dc->run(valShort.shortVal);
			}
		}
		break;

	case DIGITAL:
		{
			pinMode(pin, OUTPUT);
			int v = readBuffer(7);
			digitalWrite(pin, v);
		}
		break;

	case PWM:
		{
			pinMode(pin, OUTPUT);
			int v = readBuffer(7);
			analogWrite(pin, v);
		}
		break;
	case TONE:
		{
			pinMode(pin, OUTPUT);
			int hz = readShort(7);
			int ms = readShort(9);
			if (ms > 0) {
				tone(pin, hz, ms);
			} else {
				noTone(pin);
			}
		}
		break;
	case TIMER:
		lastTime = millis() / 1000.0;
		break;
	}
}
void readSensor(int device) {
	/**************************************************
	 ff 55 len idx action device port slot data a
	 0  1  2   3   4      5      6    7    8
	 ***************************************************/
	float value = 0.0;
	int port, slot, pin;
	port = readBuffer(6);
	pin = port;
	switch (device) {
	case VERSION: {
		sendString(mVersion);
	}
		break;
	case DIGITAL: {
		pinMode(pin, INPUT);
		sendFloat(digitalRead(pin));
	}
		break;
	case ANALOG: {
		pin = analogs[pin];
		pinMode(pin, INPUT);
		sendFloat(analogRead(pin));
	}
		break;
	case TIMER: {
		sendFloat((float) currentTime);
	}
		break;
	}
}
