#include "claw.h"

SRTClaw::SRTClaw(int _pin)
{
  pin = _pin;
}

void SRTClaw::init()
{
  pinMode(pin, OUTPUT);
  servo.attach(pin);
  servo.write(0);
}

void SRTClaw::open()
{
  angle = 0;
  servo.write(angle);
}

void SRTClaw::close()
{
  angle = 60;
  servo.write(angle);
}
