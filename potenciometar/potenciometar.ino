int ledPin = 11;
int buttonPin = 12;

int scale(int val)
{
  return (int)((val / 1024.0) * 255.0);
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int val = analogRead(A0);
 
  if (digitalRead(buttonPin) == HIGH)
  {
    analogWrite(ledPin, scale(val));
  }
  else
  {
    analogWrite(ledPin, 0);
  
  }
  Serial.println(val);
}

