#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>
 
#include <SPI.h>

#define READ  1
#define WRITE 0

//PWD   B   Y   X
//11    1  10  10

#define PWD  12
#define _DATA unsigned long

#ifndef ADI

int a=5;
int a1=6;
int a2=9;
int a3=10;
 
void setupAdi() {
  pinMode (a, OUTPUT);
  pinMode (a1, OUTPUT);
  pinMode (a2, OUTPUT);
  pinMode (a3, OUTPUT);
}
 
void move(int smjer)
{
  switch(smjer)
  {
    case 1:   //naprijed
      digitalWrite(a, HIGH);
      digitalWrite(a2, HIGH);
      break;
    case 2:   //nazad
      digitalWrite(a1, HIGH);
      digitalWrite(a3, HIGH);
      break;
    case 3:   //lijevo
     digitalWrite(a, HIGH);
     digitalWrite(a3, HIGH);
     break;
    case 4:   //desno
      digitalWrite(a1, HIGH);
      digitalWrite(a2, HIGH);
      break;
  }
}

#endif

class Coder
{
public:
  int transformX(int rawX)
  {
    return rawX;
  }

  int transformY(int rawY)
  {
    return rawY;
  }

  unsigned long encode(int rawX, int rawY, bool pressed)
  {
    int x = transformX(rawX);
    int y = transformX(rawY);
  
    unsigned long ret = PWD;
    ret <<= 1;
    ret += pressed;
    ret <<= 10;
    ret += y;
    ret <<= 10;
    ret += x;
    return ret;
  }

  int getX(unsigned long data)
  {
    unsigned long BITS = (1<<10) - 1;
    return data & BITS;
  }

  int getY(unsigned long data)
  {
    unsigned long BITS = (1<<10) - 1;
    data >>= 10;
    return data & (BITS);  
  }
};

Coder coder;

const bool radioNumber = 1;
const int kanal = 30;
RF24 radio(7, 8);
byte addresses [][6] = { "1Node","2Node" };

void serialSetup()
{
  Serial.begin(115200);
  Serial.println(F("*** Pocelo, sacekaj sinhronizaciju"));
}

void radioSetup()
{
  radio.begin();
  radio.setChannel(kanal);

  radio.setPALevel(RF24_PA_LOW);

  if(radioNumber)
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  else
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }

  radio.startListening();
}

void setup()
{
  serialSetup();
  radioSetup();
  setupAdi();
}

const unsigned long DEAD_ZONE = 5;

bool process(int x, int y)
{
  x -= 512;
  y -= 512;

  unsigned long X = x;
  unsigned long Y = y;

  if (X * X + Y * Y > DEAD_ZONE * DEAD_ZONE) {
    if (y > x && y > - x)
    {
      move(1);
      Serial.println("MOVING\n");
      return true;
    }
    if (y < x && y > - x)
    {
      move(2);
      Serial.println("MOVING\n");
      return true;
    }
    if (y > x && y < - x)
    {
      move(3);
      Serial.println("MOVING\n");
      return true;
    }
    if (y < x && y < - x)
    {
      move(4);
      Serial.println("MOVING\n");
      return true;
    }
  }

  return false;
}

void receive(void *niz,int vel)
{
  radio.stopListening();                                    // First, stop listening so we can talk.
  unsigned long start_time = micros(); // Take the time, and send it.  This will block until complete    
  radio.startListening();  
  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
  
  while (!radio.available())// While nothing is received
  {                             
    if (micros() - started_waiting_at > 200000) // If waited longer than 200ms, indicate timeout and exit while loop
    {            
        timeout = true;
        break;
    }      
  } 
  if (timeout)
  {                                             // Describe the results
      Serial.println(F("Failed, response timed out."));
  }
  else
  {
      int x, y;
      unsigned long data;                                 // Grab the response, compare, and send to debugging spew
      radio.read( &data, sizeof(data) );
      Serial.print(coder.getX(data)); 
      Serial.print(" "); 
      Serial.print(coder.getY(data)); 
      Serial.print("data: "); Serial.print(data);
      Serial.println("");
  
      x = coder.getX(data);
      y = coder.getY(data);
      bool res = process(x, y);

      if (!res)
      {
        digitalWrite(a, LOW);
        digitalWrite(a1, LOW);
        digitalWrite(a2, LOW);
        digitalWrite(a3, LOW);
      }
  }
}

int u;

void loop()
{
 
    receive(&u,sizeof(u));
 
} // Loop
