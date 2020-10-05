#include "line.h"

SRTLine::SRTLine(int _pin)
{
  pin = _pin;
}

void SRTLine::init()
{
  pinMode(pin, INPUT);
}

int SRTLine::getValue()
{
  return analogRead(pin);
}

bool SRTLine::onLine()
{
  return (getValue() > 50);
}
