#include <Arduino.h>
#include "basicIO.h"

float basicIO::digitalGet(byte port) {
	pinMode(port, INPUT);
	return (digitalRead(port) == HIGH) ? 1.0 : 0.0;
}

void basicIO::digitalSet(byte port, float value) {
	pinMode(port, OUTPUT);
	digitalWrite(port, value == 0.0 ? LOW : HIGH);
}

float basicIO::analogGet(byte port) {
	pinMode(port, INPUT);
	return (float)analogRead(port);
}

void basicIO::analogSet(byte port, float value) {
	pinMode(port, OUTPUT);
	analogWrite(port, (byte)value);
}
