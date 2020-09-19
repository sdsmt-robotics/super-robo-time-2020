#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h> // https://github.com/STEMpedia/DabbleESP32
#include <L289N.h>       // https://github.com/sdsmt-robotics/L298N
#include <analogWrite.h> // https://github.com/ERROPiX/ESP32_AnalogWrite

L289N lMotor(23, 22, 1 );
L289N rMotor(3,  21, 19);
int lVel, rVel;

void setup()
{
  Serial.begin(115200);
  Dabble.begin("Dustin's ESP");
  analogWriteFrequency(2000);
  lMotor.init();
  rMotor.init();
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
}
