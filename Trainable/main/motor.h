#include <Adafruit_PWMServoDriver.h>
#include "variables.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void moveMotor(int motorOut, int angle)
{
  int pulse_wide, pulse_width;

  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * frequency * 4096);

  pwm.setPWM(motorOut, 0, pulse_width);
  delay(servoDelay(motorOut, angle));
}

int servoDelay(int servo, int angle)
{
  int crntPos = analogRead(servo);
  crntPos = map(crntPos, servoA_Min, servoA_Max, 0, 180);
  return (abs(angle - crntPos) * (300 / 100));
}
