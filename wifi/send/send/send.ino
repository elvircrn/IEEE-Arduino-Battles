#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>

/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/
 
#include <SPI.h>

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

 
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
 
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
Coder coder;
/**********************************************************/
 
const int kanal = 30;
 
byte addresses[][6] = {"1Node","2Node"};
 
void setup() {
  Serial.begin(115200);
 
  radio.begin();
  
  radio.setChannel(kanal);
 
  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
 
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
 
  // Start the radio listening for data
  radio.startListening();
 // Serial.println(F("*** Krenulo je dalje"));
}

void loop() {
  
  int x = (int)analogRead(A0);
  int y = (int)analogRead(A1);

  unsigned long data = coder.encode(x, y, 0);

  //Serial.print("Data: "); Serial.print(coder.getX(data)); Serial.print(" "); Serial.print(coder.getY(data)); Serial.println("");
  radio.stopListening();                                    // First, stop listening so we can talk.
 
  
    if (!radio.write( &data, sizeof(data)))
    {
      Serial.println(F("failed"));
    }


}



