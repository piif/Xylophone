#include "MeDCMotor.h"

MeDCMotor::MeDCMotor() : MePort(0) {
	endTime = 0;
}

MeDCMotor::MeDCMotor(uint8_t port) : MePort(port) {
	MeDCMotor::stop();
}

void MeDCMotor::run(short duration) {
	if (duration == 0) {
		stop();
	} else if (duration > 0) {
		endTime = millis() + duration;
		dWrite1(HIGH);
		dWrite2(LOW);
	} else {
		endTime = millis() - duration;
		dWrite1(LOW);
		dWrite2(HIGH);
	}
}

void MeDCMotor::stop() {
		endTime = 0;
		dWrite1(LOW);
		dWrite2(LOW);
}

void MeDCMotor::step() {
	if (endTime != 0 && millis() > endTime) {
		stop();
	}
}

unsigned long MeDCMotor::remaining() {
	if (endTime == 0) {
		return 0;

	}
	return endTime - millis();
}
