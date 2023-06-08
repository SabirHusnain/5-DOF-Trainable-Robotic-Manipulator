#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Key.h>
#include <Keypad.h>

void moveMotor(int, int);
void moveMotorA(int, int);
void manualMove();
void getAutoMoveData();
void autoMoveForward();
void autoMoveReverse();
void getOrderData();
void getStartAngleData();
void getEndAngleData();
void getIndvMoveDelays();
void conv2Num(int []);
void displayData();

#define Rows 4
#define Cols 3

byte rowPins[Rows] = {2, 3, 4, 5};
byte colPins[Cols] = {6, 7, 8};

char keys[Rows][Cols] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};

#define MIN_PULSE_WIDTH 1000
#define MAX_PULSE_WIDTH 2002
#define frequency 50

#define potA 0
#define potB 1
#define potC 2
#define potD 3
#define potE 6
#define potF 7

#define motorA 0
#define motorB 1
#define motorC 2
#define motorD 3
#define motorE 4
#define motorF 5

#define autoBtn 12

bool autoMode = false;
bool autoDataGet=false;

int order[6];
int startAngle[6];
int endAngle[6];
int delayIndvMove[6];
int mergeNum = 0;
int i = 0;
int n = 0;
int tempNum[6];

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, Rows, Cols);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void moveMotor(int controlIn, int motorOut)
{
  int pulse_wide, pulse_width, potVal;

  potVal = analogRead(controlIn);

  potVal = map(potVal, 0, 1023, 0, 180);

  pulse_wide = map(potVal, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * frequency * 4096);

  pwm.setPWM(motorOut, 0, pulse_width);
}

void moveMotorA(int motorOut, int angle)
{
  int pulse_wide, pulse_width;

  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * frequency * 4096);

  pwm.setPWM(motorOut, 0, pulse_width);
}

void manualMove()
{
  moveMotor(potA, motorA);
  moveMotor(potB, motorB);
  moveMotor(potC, motorC);
  moveMotor(potD, motorD);
  moveMotor(potE, motorE);
  moveMotor(potF, motorF);
}

void getAutoMoveData()
{
  Serial.println("****** Enter Order ******");
  getOrderData();
  Serial.println("****** Enter Start Angle ******");
  getStartAngleData();
  Serial.println("****** Enter End Angle ******");
  getEndAngleData();
  Serial.println("****** Enter Delays ******");
  getIndvMoveDelays();
  autoDataGet=true;
  displayData();
}

void autoMoveForward()
{
  for (int mtr = 0; mtr < 6; mtr++)
  {
    Serial.print("Motor: ");
    Serial.println(mtr);
    moveMotorA(order[mtr] - 1, endAngle[mtr]);
    delay(delayIndvMove[mtr] + (abs((endAngle[mtr] - startAngle[mtr])) * (350 / 100)));
  }
}

void autoMoveReverse()
{
  for (int mtr = 5; mtr >= 0; mtr--)
  {
    moveMotorA(order[mtr] - 1, startAngle[mtr]);
    delay(delayIndvMove[mtr] + (abs((endAngle[mtr] - startAngle[mtr])) * (350 / 100)));
  }
}

void getOrderData()
{
  i = 0;
  while (i < 6)
  {
    char key = myKeypad.getKey();
    if (key != NO_KEY)
    {
      if (key != '*' || key != '#')
      {
        delay(30);
        order[i] = (int)key - 48;
        i++;
      }
    }
  }
  Serial.println("Order Saved!");
}

void getStartAngleData()
{
  i = 0;
  while (i < 6)
  {
    n = 0;
    while (true)
    {
      char key = myKeypad.getKey();
      if (key != NO_KEY)
      {
        delay(30);
        if (key == '*')
          return;
        if (key == '#')
          break;
        tempNum[n] = (int)key - 48;
        n++;
      }
    }
    conv2Num(tempNum);
    startAngle[i] = mergeNum;
    i++;
  }
  Serial.println("Start Angle has been saved!");
}

void getEndAngleData()
{
  i = 0;
  while (i < 6)
  {
    n = 0;
    while (true)
    {
      char key = myKeypad.getKey();
      if (key != NO_KEY)
      {
        delay(30);
        if (key == '*')
          return;
        if (key == '#')
          break;
        tempNum[n] = (int)key - 48;
        n++;
      }
    }
    conv2Num(tempNum);
    endAngle[i] = mergeNum;
    i++;
  }
  Serial.println("End Angle has been saved!");
}

void getIndvMoveDelays()
{
  i = 0;
  while (i < 6)
  {
    n = 0;
    while (true)
    {
      char key = myKeypad.getKey();
      if (key != NO_KEY)
      {
        delay(30);
        if (key == '*')
          return;
        if (key == '#')
          break;
        tempNum[n] = (int)key - 48;
        n++;
      }
    }
    conv2Num(tempNum);
    delayIndvMove[i] = mergeNum;
    i++;
  }
  Serial.println("Delays has been saved!");
}

void conv2Num(int num[])
{
  n--;
  mergeNum = 0;
  int j = 1;
  while (n >= 0)
  {
    mergeNum += (num[n] * j);
    n--;
    j *= 10;
  }
  n = 0;
}

void displayData()
{
  Serial.println("Order is:-");
  for (i = 0; i < 6; i++)
    Serial.println(order[i]);
  Serial.println("Start Angles are:-");
  for (i = 0; i < 6; i++)
    Serial.println(startAngle[i]);
  Serial.println("End Angles are:-");
  for (i = 0; i < 6; i++)
    Serial.println(endAngle[i]);
  Serial.println("Delay between Movements is:-");
  for (i = 0; i < 6; i++)
    Serial.println(delayIndvMove[i]);
}
