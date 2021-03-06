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
bool radioNumber = 1;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/
const int kanal = 50;
byte addresses[][6] = {"1Node","2Node"};
void setup() {
  Serial.begin(115200);
  Serial.println(F("*** Pocelo, sacekaj sinhronizaciju")); 
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
}
void Salji(void *niz,int vel)
{
  radio.stopListening();                                    // First, stop listening so we can talk.
  unsigned long start_time = micros(); // Take the time, and send it.  This will block until complete    
  radio.startListening();  
  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
  while ( ! radio.available() ){                             // While nothing is received
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
        timeout = true;
        break;
    }      
  } 
  if ( timeout ){                                             // Describe the results
      Serial.println(F("Failed, response timed out."));
  }else{
      int prim;                                 // Grab the response, compare, and send to debugging spew
      radio.read( &prim, sizeof(prim) );
      Serial.println(prim);
  }
  delay(250);
}
int u;
void loop() {
 
/****************** Ping Out Role ***************************/  
    Salji(&u,sizeof(u));
 
} // Loop
