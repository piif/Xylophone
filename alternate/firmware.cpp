
#include <Arduino.h>
#include "firmware.h"
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
	// actual buffer length must be at least 4 and match message.len byte
	if (len < 4 || len < msg->len + 3) {
		LOG("incomplete message");
		return 0;
	}
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
			LOG("TODO");
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
	if (value < 0) {
		LOG("< 0");
		return;
	}
	// end of packet or packet too big
	// in fact, if packet were not cr/lf separated, she should have to parse length
	// to avoid to block parsing
	if(value == 0x0d || value == 0x0a || bufferIndex >= MESSAGE_MAX_LEN) {
		if (bufferIndex != 0) {
			LOG("parse");
			byte read = parseMessage(buffer, bufferIndex);
			if (read != bufferIndex) {
				// TODO memcpy rest of buffer
			}
			bufferIndex = 0;
		}
		return;
	}
	// else while packet is not "started" ignore everything but ff,55
	if (bufferIndex == 0 && value != 0xff) {
		LOG("not ff");
		return;
	}
	if (bufferIndex == 1 && value != 0x55) {
		LOG("not 55");
		bufferIndex = 0;
		return;
	}
	// else append to buffer
	buffer[bufferIndex++] = value;
}

void setup() {
	Serial.begin(115200);
	delay(100);
	LOG("Setup OK");
}

void loop() {
	while (Serial.available()) {
		addInput(Serial.read());
	}
	delay(100);
}
