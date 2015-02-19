#ifndef MEDCMOTOR_H_
#define MEDCMOTOR_H_
#include "MePort.h"
///@brief Class for DC Motor Module
class MeDCMotor: public MePort
{
public:
	MeDCMotor();
    MeDCMotor(uint8_t port);
    void run(short duration);
    void stop();
    void step();
    unsigned long remaining();
private:
    unsigned long endTime;
};
#endif
