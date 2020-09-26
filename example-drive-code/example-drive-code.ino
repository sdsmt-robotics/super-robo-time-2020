/*
Super Robo Time 2020 Robot Code

Install the libraries listed below and add the esp32 board profiles from this link:
  https://dl.espressif.com/dl/package_esp32_index.json
  (paste into the additional board managers spot in File > Preferences, then go to
    Tools > Board > Boards Manager... and search for esp32)

Designed for use on the NodeMCU-32S

Author: Dustin Richards <dustin.richards@mines.sdsmt.edu>
Contributors:
  Heath Buer, fixed a very annoying crash by finding that running the motor driver
    on pins TX0 and RX0 == bad time

This code has no copyright license, do whatever you want with it
*/

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h> // https://github.com/STEMpedia/DabbleESP32
#include <L289N.h>       // https://github.com/sdsmt-robotics/L298N
#include <analogWrite.h> // https://github.com/ERROPiX/ESP32_AnalogWrite
#include <Ultrasonic.h>  // https://github.com/JRodrigoTech/Ultrasonic-HC-SR04

//motor driver setup
L289N rMotor(23, 22, 21, true);
L289N lMotor(19, 18, 5,  true);
int lVel, rVel;

//ultrasonic distance sensor setup
Ultrasonic ultrasonic(16, 17); //TRIG, ECHO
const int ULTRASONIC_NUM_SAMPLES = 20; //number of samples to average across
const int ULTRASONIC_PERIOD = 5; //milliseconds between samples
uint64_t prevTimeUltrasonic = 0; //keeps track of the last time we grabbed a sample from the ultrasonic sensor
int ultrasonicAverageIndex = 0; //keeps track of where in the rolling average array we should write to
int ultrasonicSum = 0; //don't directly read this, use the average. used in calculating the average
int ultrasonicAverage = 0; //holds the calculated rolling average from the ultrasonic sensor's samples
bool ultrasonicRun = 1; //1 to run the ultrasonic sensor, 0 to not. stop running if you need to free up some CPU cycles
int ultrasonicSamples[ULTRASONIC_NUM_SAMPLES] = {0};

//status LED!
const int BLINK_PERIOD = 200; //ms between blinks
bool ledState = 0;
uint64_t prevTimeLED = 0;

void setup()
{
  Serial.begin(115200);
  Dabble.begin("DEFAULT SRT ROBOT NAME"); //change the name inside the quotes, this will appear in your Bluetooth menu
  
  analogWriteFrequency(2000);
  lMotor.init();
  rMotor.init();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //do some math to figure out how to drive each motor
  Dabble.processInput();
  float xRaw = GamePad.getXaxisData();
  float yRaw = GamePad.getYaxisData();
  float xBias = -abs(xRaw) / 7 + 1;
  int yMap = sqrt(pow(xRaw, 2) + pow(yRaw, 2));
  yMap = map(yMap, 0, 7, 0, 255);
  if (yRaw < 0) yMap *= -1;

  lVel = yMap;
  rVel = yMap;

  if (xRaw > 0)
  {
    rVel *= xBias;
  }
  else if (xRaw < 0)
  {
    lVel *= xBias;
  }

  lMotor.setSpeedDirection(lVel);
  rMotor.setSpeedDirection(rVel);

  //handle blinking the ESP32's built-in LED
  if (millis() > prevTimeLED + BLINK_PERIOD)
  {
    digitalWrite(LED_BUILTIN, ledState);
    ledState = !ledState;
    prevTimeLED = millis();
  }

  //handle getting and averaging samples from the ultrasonic sensor
  if (ultrasonicRun)
  {
    if (millis() > prevTimeUltrasonic + ULTRASONIC_PERIOD)
    {
      ultrasonicSamples[ultrasonicAverageIndex++] = ultrasonic.Ranging(CM);
      if (ultrasonicAverageIndex >= ULTRASONIC_NUM_SAMPLES) ultrasonicAverageIndex = 0;

      ultrasonicSum = 0;
      for (int i = 0; i < ULTRASONIC_NUM_SAMPLES; i++)
      {
        ultrasonicSum += ultrasonicSamples[i];  
      }

      ultrasonicAverage = ultrasonicSum / ULTRASONIC_NUM_SAMPLES;
      
      prevTimeUltrasonic = millis();
    }
  }
}
