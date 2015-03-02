#include <Arduino.h>
#include "motor.h"
#include "callbacks.h"

#define NB_MOTORS 4

typedef struct _motor {
	byte port;
	byte leftPin; // PWM
	byte rightPin; // digital
	bool bySpeed; // true = set speed / false = set duration
	unsigned long stopAt; // timestamp to stop motor at
} Motor;
Motor motors[4] = {
	{ 0x90,  6,  7, 1, 0 }, // M1
	{ 0xa0,  5,  4, 1, 0 }, // M2
	{ 0x10, 11, 10, 0, 0 }, // Port1
	{ 0x20,  3,  9, 0, 0 }  // Port2
};

void setPwm(byte leftPin, byte rightPin, int speed) {
	pinMode(leftPin, OUTPUT);
	pinMode(rightPin, OUTPUT);
	if (speed == 0) {
		digitalWrite(leftPin, LOW);
		digitalWrite(rightPin, LOW);
	} else if (speed > 0) {
		if (speed > 255) {
			speed = 255;
		}
		analogWrite(leftPin, speed);
		digitalWrite(rightPin, LOW);
	} else {
		if (speed < -255) {
			speed = 0;
		} else {
			speed = 255 + speed;
		}
		analogWrite(leftPin, speed);
		digitalWrite(rightPin, HIGH);
	}
}

void motor::run(byte port, float speed) {
	for (byte i = 0; i < NB_MOTORS; i++) {
		if (motors[i].port == port) {
			if (motors[i].bySpeed) {
				setPwm(motors[i].leftPin, motors[i].rightPin, speed);
			} else {
				if (speed == 0) {
					setPwm(motors[i].leftPin, motors[i].rightPin, 0);
				} else {
					if (speed > 0) {
						setPwm(motors[i].leftPin, motors[i].rightPin, 255);
					} else {
						setPwm(motors[i].leftPin, motors[i].rightPin, -255);
						speed = -speed;
					}
					motors[i].stopAt = millis() + speed;
				}
			}
			return;
		}
	}
}

void callback() {
	unsigned long now = millis();
	for (byte i = 0; i < NB_MOTORS; i++) {
		if (motors[i].stopAt != 0 && motors[i].stopAt < now) {
			setPwm(motors[i].leftPin, motors[i].rightPin, 0);
			motors[i].stopAt = 0;
		}
	}
}
Callbacks::Entry cb = Callbacks::add(callback);
