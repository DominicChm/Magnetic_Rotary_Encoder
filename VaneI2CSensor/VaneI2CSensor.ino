#include <Wire.h>

const int rollingAvgLength = 3; //How many readings will be averaged for the angle
int rollingA[rollingAvgLength];
int rollingB[rollingAvgLength];

const int center = 513; //Rough center of values. Only used for initialization.
int aAvg = center;
int bAvg = center;
//Hall effect ranges, used for mapping. Auto-calibrated at beggining of code.
int aLower = 1024; 
int aUpper = 0;
int bLower = 1024;
int bUpper = 0;

int lowerThreshold = 0;

int aVector = 0;
int bVector = 0;
double angle = 0;

boolean calibrated = false;

void setup() {
  Wire.begin(45); //i2c bus Address
  Wire.onRequest(requestEvent);
  Serial.begin(57600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  pushNum(analogRead(A0), rollingA);
  pushNum(analogRead(A1), rollingB);
  aAvg = calcAvg(rollingA);
  bAvg = calcAvg(rollingB);
  if(aAvg > aUpper){aUpper = aAvg;}
  else if(aAvg < aLower){aLower = aAvg;}

  if(bAvg > bUpper){bUpper = bAvg;}
  else if(bAvg < bLower){bLower = bAvg;}

  aVector = map(aAvg, aLower, aUpper, -100, 100);
  bVector = map(bAvg, bLower, bUpper, -100, 100);

  angle = atan2((double)aVector, (double)bVector);
  angle = angle * (180.0/M_PI);

  Serial.println("Angle: " + String(angle));
  delay(10);
}

void pushNum(int n, int *arr) //pushes a number to the beginning of given array.
{
    for(int i = rollingAvgLength - 1; i > 0; i--)
    {
      arr[i] = arr[i-1];
    }
    arr[0] = n;
}

int calcAvg(int *arr) //calculates average of vals in an array.
{
  int sum = 0;
  for(int i = 0; i < rollingAvgLength; i++)
    {
      sum += arr[i];
    }
    return sum/rollingAvgLength;
}

void requestEvent()
{
  byte* bytePointer = (byte*)&angle;
  Wire.write(bytePointer, 4);
}
