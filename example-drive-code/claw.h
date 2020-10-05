#ifndef __SRT_CLAW
#define __SRT_CLAW

#define SRT_CLAW_MAX_ANGLE 55
#define SRT_CLAW_MIN_ANGLE 15

#include <Servo.h>

class SRTClaw
{
private:
  int pin;
  int openAngle = 15;
  int closedAngle = 45;
  Servo servo;

public:
  SRTClaw(int _pin);
  void init(int ledcChannel);
  void open();
  void close();
  void setAngle(int angle);
  void setOpenAngle(int _openAngle);
  void setClosedAngle(int _closedAngle);
};

#endif
