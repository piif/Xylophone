#include "MeDCMotor.h"

MeDCMotor::MeDCMotor() : MePort(0) {
}

MeDCMotor::MeDCMotor(uint8_t port) : MePort(port) {
	MeDCMotor::run(0);
}

void MeDCMotor::run(int duration) {

	if (duration == 0) {
		stop();
	} else if (duration > 0) {
			endTime = millis() + duration;
			MePort::dWrite2(HIGH);
			MePort::aWrite1(LOW);
		} else {
			endTime = millis() - duration;
			MePort::dWrite2(LOW);
			MePort::aWrite1(HIGH);
		}
	}
void MeDCMotor::stop() {
		endTime = 0;
		MePort::dWrite2(LOW);
		MePort::aWrite1(LOW);
}
void MeDCMotor::step() {
	if (millis() > endTime) {
		stop();
	}
}
