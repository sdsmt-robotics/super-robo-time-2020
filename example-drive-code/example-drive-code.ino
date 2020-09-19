#define SDSMT_SRT_ESP32
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <L289N.h> //https://github.com/sdsmt-robotics/L298N
#include <analogWrite.h> //https://github.com/ERROPiX/ESP32_AnalogWrite

L289N lMotor(23, 22, 1 );
L289N rMotor(3,  21, 19);
int lVel, rVel;
const int deadzone = 1;

void setup()
{
  Serial.begin(115200);
  Dabble.begin("Dustin's ESP");
  analogWriteFrequency(2000);
  lMotor.init();
  rMotor.init();
  /*pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);*/
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
  //int yMap = map(yRaw, -7, 7, -255, 255);
    
  /*if (GamePad.isTrianglePressed())
  {
    lVel = 255;
    rVel = 255;
  }
  else if (GamePad.isCrossPressed())
  {
    lVel = -255;
    rVel = -255;
  }
  else
  {
    lVel = 0;
    rVel = 0;
  }*/

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

  /*digitalWrite(34, HIGH);
  digitalWrite(35, HIGH);
  digitalWrite(32, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(34, LOW);
  digitalWrite(35, LOW);
  digitalWrite(32, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);*/
}
