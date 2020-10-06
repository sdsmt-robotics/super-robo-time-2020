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
#include <FastLED.h>     // https://github.com/FastLED/FastLED

#include "batterySense.h"
#include "claw.h"
#include "line.h"
#include "srt-ultrasonic.h"

//motor driver setup
L289N rMotor(23, 22, 21, true);
L289N lMotor(19, 18, 5,  true);
int lVel, rVel;

//ultrasonic distance sensor setup
Ultrasonic ultrasonic(16, 17); //TRIG, ECHO
SRTUltrasonic distance;

//status LED!
const int BLINK_PERIOD = 200; //ms between blinks
bool ledState = 0;
uint32_t prevTimeLED = 0;

//battery voltage sensor
SRTBatterySense battery(A0);

//LED strip
FASTLED_USING_NAMESPACE
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
const int DATA_PIN = 15;
const int NUM_LEDS = 8;
const int BRIGHTNESS = 40;
CRGB ledStrip[NUM_LEDS];

//claw
SRTClaw claw(10);

//infrared line sensor
SRTLine line(A3);

void setup()
{
  Serial.begin(115200);
  Dabble.begin("DEFAULT SRT ROBOT NAME"); //change the name inside the quotes, this will appear in your Bluetooth menu
  
  analogWriteFrequency(2000);
  lMotor.init();
  rMotor.init();
  claw.init(2); //claw MUST be initialized AFTER the motors on ledc channel >= 2
  line.init();
  battery.init();
  distance.init(&ultrasonic);

  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(ledStrip, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    ledStrip[i] = CRGB::Green;
  }
  FastLED.show();
}

void loop() {
  //stop the bot if the battery is low
  if (battery.getRollingAverage() < 7)
  {
    stopRobot(); 
  }

  //sample the ultrasonic sensor, this needs to run very frequently
  distance.sample();
  
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

  //set the motor speeds
  lMotor.setSpeedDirection(lVel, true);
  rMotor.setSpeedDirection(rVel, true);

  //handle blinking the ESP32's built-in LED
  if (millis() > prevTimeLED + BLINK_PERIOD)
  {
    digitalWrite(LED_BUILTIN, ledState);
    ledState = !ledState;
    prevTimeLED = millis();
  }  
}

void stopRobot()
{
  lMotor.setSpeedDirection(0);
  rMotor.setSpeedDirection(0);

  //set whole LED strip to black
  int i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    ledStrip[i] = CRGB::Black;
  }
  FastLED.show();

  i = 0;
  do
  {
    //light one LED red at a time, one after the other
    ledStrip[i++] = CRGB::Black;
    if (i >= NUM_LEDS)
    {
      i = 0;
    }
    ledStrip[i] = CRGB::Red;
    FastLED.show();

    //two short pulses on the onboard LED
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(2000);
  } while (true);
}
