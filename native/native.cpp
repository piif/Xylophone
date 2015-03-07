// quick an dirty "native" version to test Xylophone

#include <Arduino.h>

#define MOTOR_LEFT 10
#define MOTOR_RIGHT 11
#define HAMMER 3
#define HIT_DURATION 50

#define KEY_WIDTH 23.0

#define LOG(m)
//Serial.println(m)

const char *keyboard = "cdefgABCDEF";

#define MAX_BUFFER 100
char buffer[MAX_BUFFER + 1];
byte bufferIndex = 0;

long rhythm = 1000L * 60 / 60;

char melody[MAX_BUFFER];
byte counter = 0;

byte note = ' ';
short position = 0, newPosition;

unsigned long nextBeat = 0, nextHit = 0, nextMove = 0;

// find index of key c in keyboard string
short findKey(char c) {
	const char *k = keyboard;
	short result = 0;
	while(*k) {
		if (*k == c) {
			return result;
		}
		k++;
		result++;
	}
	return -1;
}

void hit() {
	digitalWrite(HAMMER, HIGH);
	delay(HIT_DURATION);
	digitalWrite(HAMMER, LOW);
}

void startHit() {
	LOG("hit");
	digitalWrite(HAMMER, HIGH);
	nextHit = millis() + HIT_DURATION;
}
void endHit() {
	nextHit = 0;
	digitalWrite(HAMMER, LOW);
}

void moveOf(short n) {
	if (n == 0) {
		return;
	}
	LOG("move ");
	LOG(n);
	unsigned long now = millis();
	if (n > 0) {
		digitalWrite(MOTOR_LEFT, LOW);
		digitalWrite(MOTOR_RIGHT, HIGH);
	} else {
		n = -n;
		digitalWrite(MOTOR_LEFT, HIGH);
		digitalWrite(MOTOR_RIGHT, LOW);
	}
	// duration = 50 + ((KEY_WIDTH * n) - 15) * 50/20
	nextMove = (unsigned long)(now + 50.0 + ((KEY_WIDTH * n) - 15.0) * 50.0/20.0);
	LOG(" -> ");
	LOG(nextMove);
}
void endMoveOf() {
	LOG("end move");
	nextMove = 0;
	digitalWrite(MOTOR_LEFT, LOW);
	digitalWrite(MOTOR_RIGHT, LOW);
}

void beat() {
	if (counter != 0 && note != ' ') {
//		hit();
	}
	if (melody[counter] == '\0') {
		nextBeat = 0;
		return;
	}
	note = melody[counter];
	counter++;
	newPosition = findKey(note);
	if (newPosition != -1) {
		moveOf(newPosition - position);
		position = newPosition;
	} else {
		note = ' ';
	}
	nextBeat += rhythm;
}

// parse an int value
int readInt(char *data) {
	int result = 0;
	while (*data != '\0') {
		if (*data >= '0' && *data <= '9') {
			result = result * 10 + *data - '0';
			data++;
		} else {
			return -1;
		}
	}
	return result;
}

void parse(char *data, byte len) {
	if (*data == '\0') {
		return;
	}
	if (data[0] == 'r') {
		// set rhythm
		int r = readInt(data + 1);
		if (r > 0) {
			rhythm = 1000L * 60 / r;
		}
	} else {
		nextBeat = nextHit = 0;
		memcpy(melody, data, len);
		melody[len] = '\0';
		LOG("play ");LOG(melody);
		counter = 0;
		nextBeat = millis() + rhythm;
	}
}

void append(int c) {
	if (bufferIndex == MAX_BUFFER || c == '\n' || c == '\r') {
		buffer[bufferIndex] = '\0';
		parse(buffer, bufferIndex);
		bufferIndex = 0;
	} else {
		buffer[bufferIndex++] = c;
	}
}

void setup() {
	Serial.begin(115200);
	pinMode(MOTOR_LEFT, OUTPUT);
	digitalWrite(MOTOR_LEFT, LOW);
	pinMode(MOTOR_RIGHT, OUTPUT);
	digitalWrite(MOTOR_RIGHT, LOW);
	pinMode(HAMMER, OUTPUT);
	digitalWrite(HAMMER, LOW);
	melody[0] = '\0';
	Serial.print("Notes are "); Serial.println(keyboard);
	Serial.println("Ready.");
}

void loop() {
	if (Serial.available()) {
		append(Serial.read());
	}
	unsigned long now = millis();
	if (nextHit != 0 && nextHit < now) {
		endHit();
	}
	if (nextMove != 0 && nextMove < now) {
		endMoveOf();
	}
	if (nextBeat != 0 && nextBeat < now) {
		beat();
	}
}
