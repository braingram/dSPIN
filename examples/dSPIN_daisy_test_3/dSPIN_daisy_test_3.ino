#include <SPI.h>
#include <dSPIN.h>

//#define CS 2
#define CS A2
#define MOSI 11
#define MISO 12
#define SCK 13

#define RESET 4

#define SET_PARAM 0x00
#define GET_PARAM 0x20
#define RESET_DEVICE 0xC0
#define GET_STATUS 0xD0

#define ABS_POS 0x01
#define CONFIG 0x18


dSPIN dspin(CS, RESET);
int b=0;


void setup() {
  // begin SPI communications
  Serial.begin(9600);
  Serial.print("starting spi...\n");
}

void configure() {
  Serial.println("resetDev");
  dspin.resetDev();
  Serial.println("configStepmode");
  dspin.configStepMode(STEP_FS);
  dspin.setMaxSpeed(1024);
  dspin.setAcc(128);
  dspin.setDec(128);
  dspin.setSlewRate(SR_530V_us);
  dspin.setOCThreshold(OC_1875mA);
  dspin.setPWMFreq(PWM_DIV_2, PWM_MUL_2);
  dspin.setOCShutdown(OC_SD_DISABLE);
  dspin.setSwitchMode(SW_USER);
  dspin.setOscMode(INT_16MHZ_OSCOUT_16MHZ);
  dspin.setAccKVAL(64);
  dspin.setDecKVAL(64);
  dspin.setRunKVAL(64);
  dspin.setHoldKVAL(16);
  dspin.setLoSpdOpt(true);
  dspin.setVoltageComp(VS_COMP_DISABLE);
}

void loop() {
  if(Serial.available() > 0) {
    b = Serial.read();
    switch (b) {
      case 99:  // c
        configure();
        break;
      case 109:  // m
        Serial.println("move");
        dspin.move(FWD, 200);
        break;
      case 114: // r
        Serial.println("move");
        dspin.move(REV, 200);
        break;
      case 102: // f
        Serial.println("forward");
        dspin.run(FWD, 400);
        break;
      case 70: // F
        Serial.println("forward");
        dspin.run(FWD, 500);
        break;
      case 98: // b
        Serial.println("back");
        dspin.run(REV, 400);
        break;
      case 66: // B
        Serial.println("back");
        dspin.run(REV, 500);
        break;
      case 115: // s
        Serial.println("stop0");
        dspin.softStop(0);
        break;
      case 83:  // S
        Serial.println("stop1");
        dspin.softStop(1);
        break;
      case 63:  // ?
        Serial.print("status=");
        Serial.print(dspin.getStatus(0), HEX);
        Serial.print(",");
        Serial.println(dspin.getStatus(1), HEX);
        break;
    }
    Serial.print("Received: ");
    Serial.println(b, DEC);
  }
  //dspin.move(FWD, 10);
  //delay(5000);
}
