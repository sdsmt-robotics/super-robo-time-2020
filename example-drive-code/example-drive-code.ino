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

//motor driver setup
L289N rMotor(23, 22, 21, true);
L289N lMotor(19, 18, 5,  true);
int lVel, rVel;

//status LED!
const int BLINK_PERIOD = 200; //ms between blinks
bool ledState = 0;
uint64_t prevTime = 0;

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
  Dabble.processInput();
  //IDEA: calculate start velocity from distance from center, bias off of that
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

  if (millis() > prevTime + BLINK_PERIOD)
  {
    digitalWrite(LED_BUILTIN, ledState);
    ledState = !ledState;
    prevTime = millis();
  }
}
