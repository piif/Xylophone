#include "MePort.h"

MePort_Sig mePort[11] = {{NC, NC}, {11, 10}, {3, 9}, {12, 13}, {8, 2},
    {NC, NC}, {A2, A3}, {A6, A1}, {A7, A0}, {6, 7}, {5, 4}
};

/*        Port       */
MePort::MePort(){
	s1 = mePort[0].s1;
    s2 = mePort[0].s2;
    _port = 0;
}
MePort::MePort(uint8_t port)
{
    s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
}
MePort::MePort(uint8_t port,uint8_t slot)
{
    s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
    _slot = slot;
}
uint8_t MePort::getPort(){
	return _port;
}
uint8_t MePort::getSlot(){
	return _slot;
}
bool MePort::dRead1()
{
    bool val;
    pinMode(s1, INPUT);
    val = digitalRead(s1);
    return val;
}

bool MePort::dRead2()
{
    bool val;
	pinMode(s2, INPUT);
    val = digitalRead(s2);
    return val;
}

void MePort::dWrite1(bool value)
{
    pinMode(s1, OUTPUT);
    digitalWrite(s1, value);
}

void MePort::dWrite2(bool value)
{
    pinMode(s2, OUTPUT);
    digitalWrite(s2, value);
}

int MePort::aRead1()
{
    int val;
    val = analogRead(s1);
    return val;
}

int MePort::aRead2()
{
    int val;
    val = analogRead(s2);
    return val;
}

void MePort::aWrite1(int value)
{   
    analogWrite(s1, value);  
}

void MePort::aWrite2(int value)
{
    analogWrite(s2, value); 
}
void MePort::reset(uint8_t port){
	s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
}
void MePort::reset(uint8_t port,uint8_t slot){
	s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
    _slot = slot;
}
uint8_t MePort::pin1(){
	return s1;
}
uint8_t MePort::pin2(){
	return s2;
}

uint8_t MePort::pin(){
	return _slot==SLOT_1?s1:s2;
}
uint8_t MePort::pin(uint8_t port,uint8_t slot){
  return slot==SLOT_1?mePort[port].s1:mePort[port].s2;
}
