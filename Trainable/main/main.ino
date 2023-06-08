#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <EEPROM.h>

void moveMotor(int, int);
void readServoPosition();
void saveServoPosition();
void readServoAngle();
void clearMemory();
int servoDelay(int, int);
void stopPlay();
void memFull();

#define MIN_PULSE_WIDTH 85
#define MAX_PULSE_WIDTH 495
#define frequency 50

#define fdbkA 0
#define fdbkB 1
#define fdbkC 2
#define fdbkD 3
#define fdbkE 6
#define fdbkF 7


#define servoA 0
#define servoB 1
#define servoC 2
#define servoD 3
#define servoE 4
#define servoF 5

#define btn_Rcd 5
#define btn_Play 4
#define btn_Stop 2
#define btn_Rst 3
#define led_Rcd 6
#define led_Rst 7
#define led_Play 8

int servoA_Max = 628;
int servoA_Min = 19;
int servoB_Max = 670;
int servoB_Min = 55;
int servoC_Max = 634;
int servoC_Min = 25;
int servoD_Max = 645;
int servoD_Min = 45;
int servoE_Max = 585;
int servoE_Min = 65;
int servoF_Max = 580;
int servoF_Min = 63;

int servo_Angle[6];

int romAddress_W = 0;
int romAddress_R = 0;

volatile bool stopFlag = false;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup()
{
  pinMode(btn_Rcd, INPUT_PULLUP);
  pinMode(btn_Play, INPUT_PULLUP);
  pinMode(btn_Stop, INPUT_PULLUP);
  pinMode(btn_Rst, INPUT_PULLUP);

  pinMode(led_Rcd, OUTPUT);
  pinMode(led_Rst, OUTPUT);
  pinMode(led_Play, OUTPUT);

  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(frequency);

  attachInterrupt(0, stopPlay, FALLING);
}

void loop()
{
  if (!digitalRead(btn_Play))
  {
    while (!digitalRead(btn_Play));
    digitalWrite(led_Rcd, LOW);
    digitalWrite(led_Play, HIGH);
    stopFlag = false;
    pwm.wakeup();
    while (true)
    {
      if (romAddress_R < EEPROM.length() && (EEPROM.read(romAddress_R) != 0 || EEPROM.read(romAddress_R + 5) != 0 || EEPROM.read(romAddress_R + 6) != 0))
      {
        if (stopFlag)
          break;
        readServoAngle();
        for (int i = 0; i < 6; i++)
          Serial.println(String(i) + " .. " + String(romAddress_R) + " .. " + String(servo_Angle[i]));
        moveMotor(servoA, servo_Angle[0]);
        moveMotor(servoB, servo_Angle[1]);
        moveMotor(servoC, servo_Angle[2]);
        moveMotor(servoD, servo_Angle[3]);
        moveMotor(servoE, servo_Angle[4]);
        moveMotor(servoF, servo_Angle[5]);
        delay(50);
      }
      else
      {
        romAddress_R = 0;
      }
    }
  }

  if (!digitalRead(btn_Rcd))
  {
    while (!digitalRead(btn_Rcd));
    pwm.sleep();
    if ((romAddress_W + 6) >= EEPROM.length())
    {
      memFull();
      memFull();
      digitalWrite(led_Rcd, HIGH);
      digitalWrite(led_Play, HIGH);
      digitalWrite(led_Rst, HIGH);
    }
    else
    {
      digitalWrite(led_Play, LOW);
      digitalWrite(led_Rcd, HIGH);
      saveServoPosition();
    }
  }

  if (!digitalRead(btn_Rst))
  {
    while (!digitalRead(btn_Rst));
    pwm.sleep();
    clearMemory();
  }
}

void moveMotor(int motorOut, int angle)
{
  int pulse_wide, pulse_width;
  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * frequency * 4096);
  pwm.setPWM(motorOut, 0, pulse_wide);
  delay(servoDelay(motorOut, angle));
}

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
  digitalWrite(led_Play, LOW);
  romAddress_W = 0;
  romAddress_R = 0;
  while (romAddress_W < EEPROM.length())
  {
    EEPROM.update(romAddress_W, 0);
    delay(10);
    romAddress_W++;
  }

  pwm.wakeup();
  delay(50);
  moveMotor(servoA, 90);
  moveMotor(servoB, 90);
  moveMotor(servoC, 90);
  moveMotor(servoD, 90);
  moveMotor(servoE, 90);
  moveMotor(servoF, 0);
  delay(50);
  pwm.sleep(); 

  romAddress_W = 0;
  romAddress_R = 0;
  digitalWrite(led_Rst, LOW);
  Serial.println();
  Serial.println("Successfuly Cleared");
  Serial.println();
}

int servoDelay(int servo, int angle)
{
  int crntPos = analogRead(servo);
  switch (servo)
  {
    case 0:
      crntPos = map(crntPos, servoA_Min, servoA_Max, 0, 180);
      break;
    case 1:
      crntPos = map(crntPos, servoB_Min, servoB_Max, 0, 180);
      break;
    case 2:
      crntPos = map(crntPos, servoC_Min, servoC_Max, 0, 180);
      break;
    case 3:
      crntPos = map(crntPos, servoD_Min, servoD_Max, 0, 180);
      break;
    case 4:
      crntPos = map(crntPos, servoE_Min, servoE_Max, 0, 180);
      break;
    case 5:
      crntPos = map(crntPos, servoF_Min, servoF_Max, 0, 180);
      break;
  }
  return (abs(angle - crntPos) * (300 / 100));
}

void stopPlay()
{
  if (!digitalRead(btn_Stop))
  {
    while (!digitalRead(btn_Stop));
    pwm.sleep();
    stopFlag = true;
  }
  else
    stopFlag = false;
}

void memFull()
{
  Serial.println("Memory full! Can't record more positions");
  digitalWrite(led_Rcd, HIGH);
  digitalWrite(led_Play, LOW);
  digitalWrite(led_Rst, LOW);
  delay(200);
  digitalWrite(led_Rcd, LOW);
  digitalWrite(led_Play, HIGH);
  digitalWrite(led_Rst, LOW);
  delay(200);
  digitalWrite(led_Rcd, LOW);
  digitalWrite(led_Play, LOW);
  digitalWrite(led_Rst, HIGH);
  delay(200);
  digitalWrite(led_Rcd, LOW);
  digitalWrite(led_Play, HIGH);
  digitalWrite(led_Rst, LOW);
  delay(200);
  digitalWrite(led_Rcd, HIGH);
  digitalWrite(led_Play, LOW);
  digitalWrite(led_Rst, LOW);
  delay(200);
  digitalWrite(led_Rcd, LOW);
  digitalWrite(led_Play, LOW);
  digitalWrite(led_Rst, LOW);
}
