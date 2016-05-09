#include <RF24_config.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <SPI.h>

#define _DATA unsigned long

unsigned char myKey [10] = { 115, 197, 144, 174, 114, 167, 121, 14, 18, 16 };

const unsigned char ftable[256] = {
    0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,
    0xaf,0xf9,0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,
    0x4e,0x38,0x44,0x28,0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,
    0x7a,0xc3,0xe9,0xfa,0x3d,0x53,0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,
    0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,0x39,0xb6,0x7b,0x0f,0xc1,0x93,
    0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,0x55,0xb9,0xda,0x85,
    0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,0x35,0xd5,
    0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
    0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,
    0x4a,0x1d,0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,
    0xde,0xc7,0x67,0x18,0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,
    0xdc,0xc9,0x5d,0x5c,0x31,0xa4,0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,
    0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,0x34,0x4b,0x1c,0x73,0xd1,0xc4,
    0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,0xad,0x04,0x23,0x9c,
    0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,0x0c,0xef,
    0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
    0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,
    0xbb,0xac,0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,
    0x05,0x59,0x2a,0x46
};


class Coder
{
public:
  static int PWD;

  unsigned short g (unsigned char *key, int k, unsigned short w) {
    unsigned char g1, g2, g3, g4, g5, g6;

    g1 = (w>>8)&0xff;
    g2 = w&0xff;

    g3 = ftable[g2 ^ key[(4*k)%10]]   ^ g1;
    g4 = ftable[g3 ^ key[(4*k+1)%10]] ^ g2;
    g5 = ftable[g4 ^ key[(4*k+2)%10]] ^ g3;
    g6 = ftable[g5 ^ key[(4*k+3)%10]] ^ g4;

    return ((g5<<8) + g6);
}

void skip32 (unsigned char key[10], unsigned char buf[4], int encrypt) {
    int  k; /* round number */
    int  i; /* round counter */
    int  kstep;
    unsigned short wl, wr;

    /* sort out direction */
    if (encrypt)
        kstep = 1, k = 0;
    else
        kstep = -1, k = 23;

    /* pack into words */
    wl = (buf[0] << 8) + buf[1];
    wr = (buf[2] << 8) + buf[3];

    /* 24 feistel rounds, doubled up */
    for (i = 0; i < 12; ++i) {
        wr ^= g(key, k, wl) ^ k;
        k += kstep;
        wl ^= g(key, k, wr) ^ k;
        k += kstep;
    }

    /* implicitly swap halves while unpacking */
    buf[0] = wr >> 8;  buf[1] = wr & 0xFF;
    buf[2] = wl >> 8;  buf[3] = wl & 0xFF;
}


  int transformX(int rawX)
  {
    return rawX;
  }

  int transformY(int rawY)
  {
    return rawY;
  }

  unsigned long encodePlain(int PWD, int rawX, int rawY, int pressed)
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

  unsigned long encode(int rawX, int rawY, bool pressed)
  {
    unsigned long ret = encodePlain(PWD, transformX(rawX), transformY(rawY), pressed);

    unsigned char BITS = ((1<<8) - 1);


    unsigned char byte0 = ret & BITS;
    unsigned char byte1 = (ret>>8) & BITS;
    unsigned char byte2 = (ret>>16) & BITS;
    unsigned char byte3 = (ret>>24) & BITS;

    unsigned char byteArray [4] = { byte3, byte2, byte1, byte0 };

    skip32(myKey, byteArray, 0);

    ret = byteArray [0];
    ret <<= 8LL;
    ret += byteArray [1];
    ret <<= 8LL;
    ret += byteArray [2];
    ret <<= 8LL;
    ret += byteArray [3];
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

  unsigned long decode(unsigned long data)
  {
    unsigned long BITS = ((1<<8) - 1);
    unsigned char byteArray [4] = { (unsigned char)((data>>24) & BITS), (unsigned char)((data>>16) & BITS),
                                    (unsigned char)((data>>8) & BITS), (unsigned char)(data & BITS) };
                                    
    

    skip32(myKey, byteArray, 1);
    //std::reverse(byteArray, byteArray + 4);
    unsigned long ret = byteArray [0];
    ret <<= 8;
    ret += byteArray [1];
    ret <<= 8;
    ret += byteArray [2];
    ret <<= 8;
    ret += byteArray [3];
    return ret;
  }

};

int Coder::PWD = 666;
byte addresses[][6] = {"1Node","2Node"};

RF24 radio(7,8);
void radioSetup(int channel, bool radioNum)
{
  radio.begin();
  radio.setChannel(channel);

  radio.setPALevel(RF24_PA_HIGH);

  if(radioNum)
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


Coder coder;

bool radioNumber = 0;

const int kanal = 30;
 
 
void setup() {
  Serial.begin(115200);
 
  radio.begin();
  
  radio.setChannel(kanal);
 
 radio.setPALevel(RF24_PA_HIGH);
 
 if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
 
  radio.startListening();
}

const int BUTTON_EPS = 10;
int prevState, currentState, currentTarget;

void kill()
{
  int arr [7] = { 10, 20, 30, 50, 102, 108, 110 };
  bool ret = false;
  for (int i = 0; i < 7; i++) {
    if (arr [i] == kanal)
      continue;
    radioSetup(arr [i], radioNumber);
    radio.stopListening();

    int data = 512000;

    if (radio.write(&data, sizeof(data)))
    {
      //Serial.print("killed: "); Serial.print(i); Serial.println("");
    }
  }
  return;
}

bool justPressed()
{
  return (currentState < BUTTON_EPS && prevState > BUTTON_EPS);
}

bool isPressed()
{
  return (currentState < BUTTON_EPS);
}

void loop()
{ 
  int x = (int)analogRead(A0);
  int y = (int)analogRead(A1);

  prevState = currentState;

  currentState = analogRead(A2);

  bool o;
  
  
  if (isPressed())
  {
    //Serial.println("killing");
    kill();
    radioSetup(kanal, radioNumber);
  }
  unsigned long data = coder.encode(x, y, o);

  radio.stopListening();
 
  
  if (!radio.write(&data, sizeof(data)))
  {
    //Serial.println(F ("failed"));
  }
}


