#include <SPI.h>
#include <dSPIN.h>

//#define CS 2
#define CS0 A2
#define CS1 2

#define MOSI 11
#define MISO 12
#define SCK 13

#define RESET 4


dSPIN dspin0(CS0, RESET);
dSPIN dspin1(CS1, RESET);
int b=0;


void setup() {
  // begin SPI communications
  Serial.begin(9600);
  Serial.print("starting spi...\n");
}

void configure(dSPIN dspin) {
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
        configure(dspin0);
        configure(dspin1);
        break;
      case 109:  // m
        Serial.println("move");
        dspin0.move(FWD, 200);
        dspin1.move(FWD, 200);
        break;
      case 114: // r
        Serial.println("move");
        dspin0.move(REV, 200);
        dspin1.move(REV, 200);
        break;
      case 102: // f
        Serial.println("forward");
        dspin0.run(FWD, 400);
        break;
      case 70: // F
        Serial.println("forward");
        dspin1.run(FWD, 400);
        break;
      case 98: // b
        Serial.println("back");
        dspin0.run(REV, 400);
        break;
      case 66: // B
        Serial.println("back");
        dspin1.run(REV, 400);
        break;
      case 115: // s
        Serial.println("stop0");
        dspin0.softStop(0);
        dspin1.softStop(0);
        break;
      case 83:  // S
        Serial.println("stop1");
        dspin0.softStop(1);
        dspin1.softStop(1);
        break;
      case 63:  // ?
        Serial.print("status=");
        Serial.print(dspin0.getStatus(0), HEX);
        Serial.print(",");
        Serial.print(dspin0.getStatus(1), HEX);
        Serial.print(",");
        Serial.print(dspin1.getStatus(0), HEX);
        Serial.print(",");
        Serial.println(dspin1.getStatus(1), HEX);
        break;
    }
    Serial.print("Received: ");
    Serial.println(b, DEC);
  }
  //dspin0.move(FWD, 10);
  //delay(5000);
}
