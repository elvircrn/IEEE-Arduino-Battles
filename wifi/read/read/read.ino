#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>
 
#include <SPI.h>

bool radioNumber = 1;

RF24 radio(7,8);
/**********************************************************/
const int kanal = 30;
byte addresses[][6] = {"1Node","2Node"};
void setup() {
  Serial.begin(115200);
  Serial.println(F("*** Pocelo, sacekaj sinhronizaciju")); 
  radio.begin();
    radio.setChannel(kanal);

  radio.setPALevel(RF24_PA_LOW);

  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }

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
 
    Salji(&u,sizeof(u));
 
} // Loop
