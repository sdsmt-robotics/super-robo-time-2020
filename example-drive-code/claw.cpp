#include "claw.h"

SRTClaw::SRTClaw(int _pin)
{
  pin = _pin;
}

void SRTClaw::init(int ledcChannel)
{
  pinMode(pin, OUTPUT);
  servo.attach(pin, ledcChannel);
}

void SRTClaw::open()
{
  setAngle(openAngle);
}

void SRTClaw::close()
{
  setAngle(closedAngle);
}

void SRTClaw::setAngle(int angle)
{
  if (angle > SRT_CLAW_MAX_ANGLE)
  {
    angle = SRT_CLAW_MAX_ANGLE;
  }
  else if (angle < SRT_CLAW_MIN_ANGLE)
  {
    angle = SRT_CLAW_MIN_ANGLE;
  }

  servo.write(angle);
}

void SRTClaw::setOpenAngle(int _openAngle)
{
  openAngle = _openAngle;
}

void SRTClaw::setClosedAngle(int _closedAngle)
{
  closedAngle = _closedAngle;
}
