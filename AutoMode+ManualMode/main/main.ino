#include "myOwnProject.h"

void setup()
{
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(frequency);
  pinMode(autoBtn, INPUT_PULLUP);
}

void loop()
{
  if (digitalRead(autoBtn) == HIGH)
    manualMove();
  else
  {
    if (!autoDataGet)
      getAutoMoveData();
    autoMoveForward();
    autoMoveReverse();
  }
}
