#include "srt-ultrasonic.h"

SRTUltrasonic::SRTUltrasonic()
{
  ;
}

void SRTUltrasonic::init(Ultrasonic* _ultrasonic)
{
  ultrasonic = _ultrasonic;
}

void SRTUltrasonic::sample()
{
  if (ultrasonicRun)
  {
    if (millis() > ultrasonicPrevTime + ULTRASONIC_PERIOD)
    {
      ultrasonicSamples[ultrasonicAverageIndex++] = ultrasonic->Ranging(CM);
      if (ultrasonicAverageIndex >= ULTRASONIC_NUM_SAMPLES) ultrasonicAverageIndex = 0;

      ultrasonicSum = 0;
      for (int i = 0; i < ULTRASONIC_NUM_SAMPLES; i++)
      {
        ultrasonicSum += ultrasonicSamples[i];  
      }

      ultrasonicAverage = ultrasonicSum / ULTRASONIC_NUM_SAMPLES;
      
      ultrasonicPrevTime = millis();
    }
  }

  return;
}

int SRTUltrasonic::getDistanceCM()
{
  return ultrasonicAverage;
}

void SRTUltrasonic::stop()
{
  ultrasonicRun = 0;
}

void SRTUltrasonic::start()
{
  ultrasonicRun = 1;
}
