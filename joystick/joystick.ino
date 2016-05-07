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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  int x = (int)analogRead(A0);
  int y = (int)analogRead(A1);

  unsigned long data = coder.encode(x, y, 0);

  Serial.print("Data: "); Serial.print(coder.getX(data)); Serial.print(" "); Serial.print(coder.getY(data)); Serial.println("");

 
}
