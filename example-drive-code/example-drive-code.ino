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
#include <FastLED.h>

FASTLED_USING_NAMESPACE

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

//LED strip
#define DATA_PIN    15
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    8
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup()
{
  Serial.begin(115200);
  Dabble.begin("DEFAULT SRT ROBOT NAME"); //change the name inside the quotes, this will appear in your Bluetooth menu
  
  analogWriteFrequency(2000);
  lMotor.init();
  rMotor.init();

  pinMode(LED_BUILTIN, OUTPUT);

  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

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

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
