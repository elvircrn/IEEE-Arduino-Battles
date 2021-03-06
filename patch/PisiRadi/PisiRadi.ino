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
 
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;
 
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/
const int kanal = 110;
byte addresses[][6] = {"1Node","2Node"};
void setup() {
  Serial.begin(115200);
 
  radio.begin();
  
  radio.setChannel(30);
 
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
 
void Salji(void *niz,int vel)
{
     
  radio.stopListening();                                    // First, stop listening so we can talk.
 
  Serial.println(F("Now sending"));
  int prim=15;
   if (!radio.write( &prim, sizeof(prim))){
     Serial.println(F("failed"));
   }
     
  radio.startListening();                                    // Now, continue listening
 
  // Try again 1s later
  delay(250);
 
}
int u=45;
void loop() {
 
/****************** Ping Out Role ***************************/  
 // if(Serial.available())
    Salji(&u,sizeof(u));
 
} // Loop
