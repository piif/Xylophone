#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	#include "serialInput/serialInput.h"
#else
	#include "serialInput.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define RIGHT 9
#define LEFT 10

#define HAMMER 11

#define SENSOR_A A0
#define SENSOR_B A1

long position = 0;

void status() {
	Serial.print("position : "); Serial.println(position);
}

int power = 128;

void move(int len) {
	if (len > 0) {
		analogWrite(LEFT, 0);
		analogWrite(RIGHT, power);
	} else {
		analogWrite(LEFT, power);
		analogWrite(RIGHT, 0);
		len = -len;
	}
	delay(len);
	analogWrite(LEFT, 0);
	analogWrite(RIGHT, 0);
}

void moveOf(int len) {
	long d;
	if (len > 0) {
		d = 50 + (len - 50) * 50 / 22;
		digitalWrite(LEFT, LOW);
		digitalWrite(RIGHT, HIGH);
	} else {
		d = 50 + (50 - len) * 50 / 22;
		digitalWrite(LEFT, LOW);
		digitalWrite(RIGHT, HIGH);
	}
	delay(d);
	digitalWrite(LEFT, LOW);
	digitalWrite(RIGHT, LOW);
}

void hit(int duration) {
	analogWrite(HAMMER, power);
//	digitalWrite(HAMMER, HIGH);
	delay(duration);
	for(byte p = power; p > 0; p--) {
		analogWrite(HAMMER, power);
	}
	digitalWrite(HAMMER, LOW);
}

InputItem inputs[] = {
	{ 'm', 'I', (void *)move },
	{ 'l', 'I', (void *)moveOf },
	{ 'h', 'I', (void *)hit },
	{ 'p', 'i', &power },
	{ '?', 'f', (void *)status }
};

void setup(void) {
	Serial.begin(DEFAULT_BAUDRATE);
	Serial.println("setup");

	pinMode(LEFT, OUTPUT);
	pinMode(RIGHT, OUTPUT);
	pinMode(HAMMER, OUTPUT);
	pinMode(SENSOR_A, INPUT);
	pinMode(SENSOR_B, INPUT);

	digitalWrite(LEFT, LOW);
	digitalWrite(RIGHT, LOW);
	digitalWrite(HAMMER, LOW);

	registerInput(sizeof(inputs), inputs);
	Serial.println("ready");
}

int A, B;

void loop() {
	handleInput();
//	A = analogRead(SENSOR_A);
//	B = analogRead(SENSOR_B);
//	Serial.print(A); Serial.print('\t'); Serial.println(B);
//	delay(100);
}
