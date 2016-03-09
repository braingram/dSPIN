#include <SPI.h>
#include "dSPIN.h"
#include "util/delay.h" // Turns out, using the Arduino "delay" function
                        //  in a library constructor causes the program to
                        //  hang if the constructor is invoked outside of
                        //  setup() or hold() (i.e., the user attempts to
                        //  create a global of the class.

// Constructors
dSPIN::dSPIN(int CSPin, int resetPin, int busyPin)
{
  _CSPin = CSPin;
  _cs_mask = digitalPinToBitMask(_CSPin);
  _cs_port = digitalPinToPort(_CSPin);
  _cs_reg = portOutputRegister(_cs_port);
  _resetPin = resetPin;
  _busyPin = busyPin;
  
  SPIConfig();
}

dSPIN::dSPIN(int CSPin, int resetPin)
{
  _CSPin = CSPin;
  _cs_mask = digitalPinToBitMask(_CSPin);
  _cs_port = digitalPinToPort(_CSPin);
  _cs_reg = portOutputRegister(_cs_port);
  _resetPin = resetPin;
  _busyPin = -1;

  SPIConfig();
}

void dSPIN::SPIConfig()
{
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(_CSPin, OUTPUT);
  digitalWrite(_CSPin, HIGH);
  pinMode(_resetPin, OUTPUT);
  if (_busyPin != -1) pinMode(_busyPin, INPUT_PULLUP);
 
  SPI.begin();
  
  SPISettings settings(5000000, MSBFIRST, SPI_MODE3); 
 
  
  digitalWrite(_resetPin, LOW);
  _delay_ms(5);
  digitalWrite(_resetPin, HIGH);
  _delay_ms(5);

  resetBuffers();
}


int dSPIN::getBusy()
{
  if (_busyPin == -1)
  {
    // check all NDSPINS
    for (byte i=0; i<NDSPINS; i++) {
      // if any are busy, return busy
      if (getBusy(i)) return 1;
    }
    // if not, return not busy
    return 0;
  }
  else
  {
    if (digitalRead(_busyPin) == HIGH)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
}

int dSPIN::getBusy(byte index)
{
  if (getParam(STATUS, index) & 0x0002)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
