#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	#include "serialInput/serialInput.h"
#else
	#include "serialInput.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define RIGHT 8
#define LEFT 9

#define HAMMER 10

#define SENSOR_A 2
#define SENSOR_B 3

long position = 0;

void status() {
	Serial.print("position : "); Serial.println(position);
}

void move(int len) {
	if (len > 0) {
		digitalWrite(LEFT, LOW);
		digitalWrite(RIGHT, HIGH);
	} else {
		digitalWrite(LEFT, HIGH);
		digitalWrite(RIGHT, LOW);
		len = -len;
	}
	delay(len);
	digitalWrite(LEFT, LOW);
	digitalWrite(RIGHT, LOW);
}

InputItem inputs[] = {
	{ 'm', 'I', (void *)move },
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

void loop() {
	handleInput();
}
