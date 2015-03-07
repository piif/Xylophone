
#include <Arduino.h>
#include "firmware.h"
#include "callbacks.h"
#include "basicIO.h"
#include "motor.h"
#include "play.h"

byte bufferIndex = 0;
byte buffer[MESSAGE_MAX_LEN];

#define LOG(m) Serial.println(/*"* "*/ m)

// send result message
void sendResult(byte index, float value) {
	resultMessage result;
	result.header = 0x55ff;
	result.index = index;
	result.value = value;
	result.suffix[0] = '\r';
	result.suffix[1] = '\n';
	Serial.write((byte *)&result, sizeof(result));
}

// parse incoming buffer and launches according actions
byte  parseMessage(byte *buffer, byte len) {
	message *msg = (message *)buffer;
	// TODO : add length validation, according to message type
	if (msg->kind == MESSAGE_GET) {
		float result;
		switch(msg->payload.get.portType) {
		case CMD_DIGITAL:
			result = basicIO::digitalGet(msg->payload.get.port);
			sendResult(msg->payload.get.index, result);
			break;
		case CMD_ANALOG:
			result = basicIO::analogGet(msg->payload.get.port);
			sendResult(msg->payload.get.index, result);
			break;
		default:
			LOG("bad port type");
		}

	} else if (msg->kind == MESSAGE_CMD) {

		switch(msg->payload.cmd) {
		case CMD_DIGITAL:
			basicIO::digitalSet(msg->payload.write.port, msg->payload.write.value);
			break;
		case CMD_ANALOG:
			basicIO::analogSet(msg->payload.write.port, msg->payload.write.value);
			break;
		case CMD_MOTOR:
			motor::run(msg->payload.write.port, msg->payload.write.value);
			break;
		case CMD_PLAY:
			LOG("TODO");
			play::tone(msg->payload.play.port,
					msg->payload.play.frequency, msg->payload.play.duration);
			break;
		default:
			LOG("bad command");
		}

	} else {
		LOG("bad kind");
	}
	return msg->len + 3;
}

// add incoming byte to current buffer and make minimal validation on it
// silently ignoring garbage
void addInput(int value) {
	static byte toRead = 0;

	if (value < 0) {
		LOG("< 0");
		return;
	}
	// while packet is not "started" ignore everything but ff,55
	// (including cr and lf)
	if (bufferIndex == 0 && value != 0xff) {
		if (value != '\r' && value != '\n') {
			// warn on garbage
			LOG("not ff");
		}
		return;
	}
	if (bufferIndex == 1 && value != 0x55) {
		LOG("not 55");
		bufferIndex = 0;
		return;
	}
	// if not garbage, append to buffer
	buffer[bufferIndex++] = value;
	// if it's "len" byte, use it to know how many bytes we have to read
	if (bufferIndex == 4) {
		toRead = value + 4;
	} else {
		// end of message => parse it
		if (bufferIndex == toRead) {
			LOG("parse");
			parseMessage(buffer, bufferIndex);
			// reset buffer
			bufferIndex = toRead = 0;
		}
	}
}

void setup() {
	// force all outputs to 0 to avoid strange states at startup
	for(byte p = 2; p <= 13; p++) {
		pinMode(p, OUTPUT);
		digitalWrite(p, LOW);
	}
	for(byte p = A0; p <= A5; p++) {
		pinMode(p, OUTPUT);
		digitalWrite(p, LOW);
	}
	Serial.begin(115200);
	LOG("Setup OK");
}

void loop() {
	while (Serial.available()) {
		addInput(Serial.read());
	}
	Callbacks::call();
}
