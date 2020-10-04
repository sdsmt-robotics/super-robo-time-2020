#ifndef __SRT_CLAW
#define __SRT_CLAW

#include <Servo.h>

class SRTClaw
{
private:
  int pin;
  int value = 0;
  Servo servo;

public:
  SRTClaw(int _pin);
  void init();
  void open();
  void close();
};

#endif
