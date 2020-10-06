#ifndef __SRT_ULTRASONIC
#define __SRT_ULTRASONIC

#include "Arduino.h"
#include "Ultrasonic.h"

class SRTUltrasonic
{
private:
  Ultrasonic* ultrasonic;
  static const int ULTRASONIC_NUM_SAMPLES = 20; //number of samples to average across
  const int ULTRASONIC_PERIOD = 1; //milliseconds between samples
  uint64_t ultrasonicPrevTime = 0; //keeps track of the last time we grabbed a sample from the ultrasonic sensor
  int ultrasonicAverageIndex = 0; //keeps track of where in the rolling average array we should write to
  int ultrasonicSum = 0; //sum of ultrasonicSamples, used in calculating the average
  int ultrasonicAverage = 0; //holds the calculated rolling average from the ultrasonic sensor's samples
  bool ultrasonicRun = 1; //1 to run the ultrasonic sensor, 0 to not. stop running if you need to free up some CPU cycles
  int ultrasonicSamples[ULTRASONIC_NUM_SAMPLES] = {0}; //holds the samples used to calculate the average

public:
  SRTUltrasonic();
  void init(Ultrasonic* _ultrasonic);
  void sample();
  int getDistanceCM();
  void stop();
  void start();
};

#endif
