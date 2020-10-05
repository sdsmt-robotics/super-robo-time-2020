#ifndef __SRT_LINE
#define __SRT_LINE

#include <Arduino.h>

class SRTLine
{
private:
  int pin;

public:
  SRTLine(int _pin);
  void init();
  int getValue();
  bool onLine();
};

#endif
