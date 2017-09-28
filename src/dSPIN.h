#ifndef dSPIN_h
#define dSPIN_h

#include "Arduino.h"
#include "dSPINConstants.h"

#ifndef NDSPINS
  // define the number of dSPIN (L6470) chips daisy chained per CS line
  // different CS lines can have multiple chips however all lines must
  // have the same number of chips
  //
  // For example, the X-NUCLEO-IHM02A1 stepper shield has 2x daisy-chained
  // L6470 chips. For 1 shield, leave NDSPINS as 2. These shields can be
  // stacked with each shield having it's own chip select. For multiple
  // shields, leave NDSPINS as 2 and make multiple instances of dSPIN
  // (1 per CS line).
  #define NDSPINS 2
#endif

#if !defined(ARDUINO_ARCH_SAM) && !defined(ARDUINO_ARCH_SAMD) && !defined(ESP8266) && !defined(ARDUINO_ARCH_STM32F2)
#define REG_TYPE byte
#else
#define REG_TYPE unsigned int
#endif
//#define DSPIN_DEBUG 1

class dSPIN
{
  // Interface to control multiple dSPIN chips daisy chained on a single
  // CS line. dSPINs are ordered with index 0 being the last chip in the
  // chain and NDSPINS-1 the first
  // see NDSPINS to configure the number of chips per chain
  public:
    // Constructors. We'll ALWAYS want a CS pin and a reset pin, but we may
    //  not want a busy pin. By using two constructors, we make it easy to
    //  allow that.
    dSPIN(int CSPin, int resetPin, int busyPin);
    dSPIN(int CSPin, int resetPin);


    //int busyCheck();
    int getBusy();
    int getBusy(byte index);
    int getStatus(byte index);

    // Provide low level access to functions that setup and transfer
    // command and value buffers to the dSPINs
    void setCommand(byte command, byte index);
    void setCommand(byte command);
    void setValue(unsigned long value, byte index);
    void setValue(unsigned long value);
    void setValueByte(byte offset, byte value, byte index);
    void setValueByte(byte offset, byte value);
    void setNBytes(byte n, byte index);
    void setNBytes(byte n);
    void setNBytes();
    // Transfer currently queued commands
    void transfer();
    unsigned long getValue(byte index);
    unsigned long getValue();
    // clear buffers after a transfer, will overwrite any results
    void resetBuffers();

    // Provide intermediate access to commands and params
    void runCommand(byte command, unsigned long value, byte index);
    void runCommand(byte command, unsigned long value);
    void runCommand(byte command, byte index);
    void runCommand(byte command);

    void setParam(byte param, unsigned long value, byte index);
    void setParam(byte param, unsigned long value);
    unsigned long getParam(byte param, byte index);

    // Configuration commands
    void configSyncPin(byte pinFunc, byte syncSteps, byte index);
    void configSyncPin(byte pinFunc, byte syncSteps);
    void configStepMode(byte stepMode, byte index);
    void configStepMode(byte stepMode);
    void setMaxSpeed(float stepsPerSecond, byte index);
    void setMaxSpeed(float stepsPerSecond);
    void setMinSpeed(float stepsPerSecond, byte index);
    void setMinSpeed(float stepsPerSecond);
    void setFullSpeed(float stepsPerSecond, byte index);
    void setFullSpeed(float stepsPerSecond);
    void setAcc(float stepsPerSecondPerSecond, byte index);
    void setAcc(float stepsPerSecondPerSecond);
    void setDec(float stepsPerSecondPerSecond, byte index);
    void setDec(float stepsPerSecondPerSecond);
    void setLoSpdOpt(boolean enable, byte index);
    void setLoSpdOpt(boolean enable);
    void setOCThreshold(byte threshold, byte index);
    void setOCThreshold(byte threshold);
    void setStallThreshold(byte threshold, byte index);
    void setStallThreshold(byte threshold);
    void setPWMFreq(int divisor, int multiplier, byte index);
    void setPWMFreq(int divisor, int multiplier);
    void setSlewRate(int slewRate, byte index);
    void setSlewRate(int slewRate);
    void setOCShutdown(int OCShutdown, byte index);
    void setOCShutdown(int OCShutdown);
    void setVoltageComp(int vsCompMode, byte index);
    void setVoltageComp(int vsCompMode);
    void setSwitchMode(int switchMode, byte index);
    void setSwitchMode(int switchMode);
    void setOscMode(int oscillatorMode, byte index);
    void setOscMode(int oscillatorMode);
    void setAccKVAL(byte kvalInput, byte index);
    void setAccKVAL(byte kvalInput);
    void setDecKVAL(byte kvalInput, byte index);
    void setDecKVAL(byte kvalInput);
    void setRunKVAL(byte kvalInput, byte index);
    void setRunKVAL(byte kvalInput);
    void setHoldKVAL(byte kvalInput, byte index);
    void setHoldKVAL(byte kvalInput);

    boolean getLoSpdOpt(byte index);
    // getSyncPin
    byte getStepMode(byte index);
    float getMaxSpeed(byte index);
    float getMinSpeed(byte index);
    float getFullSpeed(byte index);
    float getAcc(byte index);
    float getDec(byte index);
    byte getOCThreshold(byte index);
    byte getStallThreshold(byte index);
    int getPWMFreqDivisor(byte index);
    int getPWMFreqMultiplier(byte index);
    int getSlewRate(byte index);
    int getOCShutdown(byte index);
    int getVoltageComp(byte index);
    int getSwitchMode(byte index);
    int getOscMode(byte index);
    byte getAccKVAL(byte index);
    byte getDecKVAL(byte index);
    byte getRunKVAL(byte index);
    byte getHoldKVAL(byte index);

    // Operational commands.
    long getPos(byte index);
    float getSpeed(byte index);
    byte getDir(byte index);
    long getMark(byte index);
    void run(byte dir, float stepsPerSec, byte index);
    void run(byte dir, float stepsPerSec);
    void stepClock(byte dir, byte index);
    void stepClock(byte dir);
    void move(byte dir, unsigned long numSteps, byte index);
    void move(byte dir, unsigned long numSteps);
    void goTo(long pos, byte index);
    void goTo(long pos);
    void goToDir(byte dir, long pos, byte index);
    void goToDir(byte dir, long pos);
    void goUntil(byte action, byte dir, float stepsPerSec, byte index);
    void goUntil(byte action, byte dir, float stepsPerSec);
    void releaseSw(byte action, byte dir, byte index);
    void releaseSw(byte action, byte dir);
    void goHome(byte index);
    void goHome();
    void goMark(byte index);
    void goMark();
    void setMark(long newMark, byte index);
    void setMark(long newMark);
    void setPos(long newPos, byte index);
    void setPos(long newPos);
    void resetPos(byte index);
    void resetPos();
    void resetDev(byte index);
    void resetDev();
    void softStop(byte index);
    void softStop();
    void hardStop(byte index);
    void hardStop();
    void softHiZ(byte index);
    void softHiZ();
    void hardHiZ(byte index);
    void hardHiZ();

  private:
    void SPIConfig();

    // Support functions for converting from user units to L6470 units
    unsigned long accCalc(float stepsPerSecPerSec);
    unsigned long decCalc(float stepsPerSecPerSec);
    unsigned long minSpdCalc(float stepsPerSec);
    unsigned long maxSpdCalc(float stepsPerSec);
    unsigned long FSCalc(float stepsPerSec);
    unsigned long intSpdCalc(float stepsPerSec);
    unsigned long spdCalc(float stepsPerSec);

    // Support functions for converting from L6470 to user units
    float accParse(unsigned long stepsPerSecPerSec);
    float decParse(unsigned long stepsPerSecPerSec);
    float minSpdParse(unsigned long stepsPerSec);
    float maxSpdParse(unsigned long stepsPerSec);
    float FSParse(unsigned long stepsPerSec);
    float intSpdParse(unsigned long stepsPerSec);
    float spdParse(unsigned long stepsPerSec);

    // Functions to change CS
    void lowerCS();
    void raiseCS();

    REG_TYPE _cs_mask;
    REG_TYPE _cs_port;
    volatile REG_TYPE * _cs_reg;
    int _CSPin;
    int _resetPin;
    int _busyPin;

    // Transfer buffers for daisy-chained SPI messages
    unsigned long values[NDSPINS];
    byte commands[NDSPINS];
    byte nBytes[NDSPINS];
};


// User constants for public functions.

// dSPIN direction options: functions that accept dir as an argument can be
//  passed one of these constants. These functions are:
//    run()
//    stepClock()
//    move()
//    goToDir()
//    goUntil()
//    releaseSw()
#define FWD  0x01
#define REV  0x00

// dSPIN action options: functions that accept action as an argument can be
//  passed one of these constants. The contents of ABSPOS will either be
//  reset or copied to MARK, depending on the value sent. These functions are:
//    goUntil()
//    releaseSw()
#define RESET_ABSPOS  0x00
#define COPY_ABSPOS   0x01

// configSyncPin() options: the !BUSY/SYNC pin can be configured to be low when
//  the chip is executing a command, *or* to output a pulse on each full step
//  clock (with some divisor). These 
#define BUSY_PIN   0x00     // !BUSY/SYNC pin set to !BUSY mode
#define SYNC_PIN   0x80     // pin set to SYNC mode

// divisors for SYNC pulse outputs
#define SYNC_FS_2  0x00   // two per full step
#define SYNC_FS    0x10   // one per full step
#define SYNC_2FS   0x20   // one per two full steps
#define SYNC_4FS   0x30   // one per four full steps
#define SYNC_8FS   0x40   // one per eight full steps
#define SYNC_16FS  0x50   // one per 16 full steps
#define SYNC_32FS  0x60   // one per 32 full steps
#define SYNC_64FS  0x70   // one per 64 full steps

// configStepMode() options: select the microsteps per full step.
#define STEP_FS    0x00   // one step per full step
#define STEP_FS_2  0x01   // two microsteps per full step
#define STEP_FS_4  0x02   // four microsteps per full step
#define STEP_FS_8  0x03   // etc.
#define STEP_FS_16 0x04
#define STEP_FS_32 0x05
#define STEP_FS_64 0x06
#define STEP_FS_128 0x07

// setOCThreshold() options
#define OC_375mA  0x00
#define OC_750mA  0x01
#define OC_1125mA 0x02
#define OC_1500mA 0x03
#define OC_1875mA 0x04
#define OC_2250mA 0x05
#define OC_2625mA 0x06
#define OC_3000mA 0x07
#define OC_3375mA 0x08
#define OC_3750mA 0x09
#define OC_4125mA 0x0A
#define OC_4500mA 0x0B
#define OC_4875mA 0x0C
#define OC_5250mA 0x0D
#define OC_5625mA 0x0E
#define OC_6000mA 0x0F

// PWM Multiplier and divisor options
#define PWM_MUL_0_625           (0x00)<<10
#define PWM_MUL_0_75            (0x01)<<10
#define PWM_MUL_0_875           (0x02)<<10
#define PWM_MUL_1               (0x03)<<10
#define PWM_MUL_1_25            (0x04)<<10
#define PWM_MUL_1_5             (0x05)<<10
#define PWM_MUL_1_75            (0x06)<<10
#define PWM_MUL_2               (0x07)<<10
#define PWM_DIV_1               (0x00)<<13
#define PWM_DIV_2               (0x01)<<13
#define PWM_DIV_3               (0x02)<<13
#define PWM_DIV_4               (0x03)<<13
#define PWM_DIV_5               (0x04)<<13
#define PWM_DIV_6               (0x05)<<13
#define PWM_DIV_7               (0x06)<<13

// Slew rate options
#define SR_180V_us              0x0000  // 180V/us
#define SR_290V_us              0x0200  // 290V/us
#define SR_530V_us              0x0300  // 530V/us

// Overcurrent bridge shutdown options
#define OC_SD_DISABLE           0x0000  // Bridges do NOT shutdown on OC detect
#define OC_SD_ENABLE            0x0080  // Bridges shutdown on OC detect

// Voltage compensation settings. See p 34 of datasheet.
#define VS_COMP_DISABLE         0x0000  // Disable motor voltage compensation.
#define VS_COMP_ENABLE          0x0020  // Enable motor voltage compensation.

// External switch input functionality.
#define SW_HARD_STOP            0x0000 // Default; hard stop motor on switch.
#define SW_USER                 0x0010 // Tie to the GoUntil and ReleaseSW
                                       //  commands to provide jog function.
                                       //  See page 25 of datasheet.
                                                   
// Clock functionality
#define INT_16MHZ               0x0000 // Internal 16MHz, no output
#define INT_16MHZ_OSCOUT_2MHZ   0x0008 // Default; internal 16MHz, 2MHz output
#define INT_16MHZ_OSCOUT_4MHZ   0x0009 // Internal 16MHz, 4MHz output
#define INT_16MHZ_OSCOUT_8MHZ   0x000A // Internal 16MHz, 8MHz output
#define INT_16MHZ_OSCOUT_16MHZ  0x000B // Internal 16MHz, 16MHz output
#define EXT_8MHZ_XTAL_DRIVE     0x0004 // External 8MHz crystal
#define EXT_16MHZ_XTAL_DRIVE    0x0005 // External 16MHz crystal
#define EXT_24MHZ_XTAL_DRIVE    0x0006 // External 24MHz crystal
#define EXT_32MHZ_XTAL_DRIVE    0x0007 // External 32MHz crystal
#define EXT_8MHZ_OSCOUT_INVERT  0x000C // External 8MHz crystal, output inverted
#define EXT_16MHZ_OSCOUT_INVERT 0x000D // External 16MHz crystal, output inverted
#define EXT_24MHZ_OSCOUT_INVERT 0x000E // External 24MHz crystal, output inverted
#define EXT_32MHZ_OSCOUT_INVERT 0x000F // External 32MHz crystal, output inverted 
#endif

