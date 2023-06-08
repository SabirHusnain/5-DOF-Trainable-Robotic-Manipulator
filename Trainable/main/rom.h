#include "motor.h"
#include <EEPROM.h>

void readServoPosition();
void saveServoPosition();
void readServoAngle();
void clearMemory();
void stopPlay();

#define btn_Rcd 5
#define btn_Play 4
#define btn_Stop 2
#define btn_Rst 3
#define led_Rcd 6
#define led_Rst 7
#define led_Full 8

int romAddress_W = 0;
int romAddress_R = 0;

void readServoPosition()
{
  int i = 0;
  servo_Angle[i] = analogRead(fdbkA);
  servo_Angle[i] = map(servo_Angle[i], servoA_Min, servoA_Max, 0, 180);
  i++;
  servo_Angle[i] = analogRead(fdbkB);
  servo_Angle[i] = map(servo_Angle[i], servoB_Min, servoB_Max, 0, 180);
  i++;
  servo_Angle[i] = analogRead(fdbkC);
  servo_Angle[i] = map(servo_Angle[i], servoC_Min, servoC_Max, 0, 180);
  i++;
  servo_Angle[i] = analogRead(fdbkD);
  servo_Angle[i] = map(servo_Angle[i], servoD_Min, servoD_Max, 0, 180);
  i++;
  servo_Angle[i] = analogRead(fdbkE);
  servo_Angle[i] = map(servo_Angle[i], servoE_Min, servoE_Max, 0, 180);
  i++;
  servo_Angle[i] = analogRead(fdbkF);
  servo_Angle[i] = map(servo_Angle[i], servoF_Min, servoF_Max, 0, 180);
}

void saveServoPosition()
{
  Serial.println("Saving...");
  readServoPosition();
  for (int i = 0; i < 6; i++)
    Serial.println(servo_Angle[i]);
  for (int i = 0; i < 6; i++)
  {
    EEPROM.update(romAddress_W++, servo_Angle[i]);
    delay(10);
  }
  Serial.println("Saved...");
}

void readServoAngle()
{
  for (int i = 0; i < 6; i++)
  {
    servo_Angle[i] = EEPROM.read(romAddress_R++);
    delay(10);
  }
}

void clearMemory()
{
  digitalWrite(led_Rst, HIGH);
  digitalWrite(led_Rcd, LOW);
  digitalWrite(led_Full, LOW);
  romAddress_W = 0;
  romAddress_R = 0;
  while (romAddress_W < EEPROM.length())
  {
    EEPROM.update(romAddress_W, 0);
    delay(10);
    romAddress_W++;
  }

  moveMotor(servoA, 0);
  moveMotor(servoB, 0);
  moveMotor(servoC, 0);
  moveMotor(servoD, 0);
  moveMotor(servoE, 0);
  moveMotor(servoF, 0);

  delay(50);
  romAddress_W = 0;
  romAddress_R = 0;
  digitalWrite(led_Rst, LOW);
  Serial.println();
  Serial.println("Successfuly Cleared");
  Serial.println();
}

void stopPlay()
{
  if (!digitalRead(btn_Stop))
  {
    while (!digitalRead(btn_Stop));
    stopFlag = true;
  }
  else
    stopFlag = false;
}
