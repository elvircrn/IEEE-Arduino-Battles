#include <Servo.h>
#include "RF24.h"

#define METER_MAX 1024
#define SERVO_MAX 180

bool state = false;

int scale(int val, int current, int target)
{
  return (int)((float)(val / (float)current) * (float) target);
}

int currentPos;

Servo myservo;

int pos = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  myservo.attach(9);
  currentPos = 0;
}

void loop() {
  int meter = analogRead(A0);
  int scaledSpeed = scale(meter, METER_MAX, SERVO_MAX);
  scaledSpeed = 1;

  if (state == true)
  {
    currentPos += scaledSpeed;
    if (currentPos > 180)
    {
      state = false;
      currentPos = 180;
    }
  }
  else
  {
    currentPos -= scaledSpeed;
    if (currentPos < 0)
    {
      state = true;
      currentPos = 0;
    }
  }

  myservo.write(currentPos);
  delay(scale(meter, METER_MAX, 1000));

  Serial.println(scaledSpeed);
}
