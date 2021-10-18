#include "dSPIN.h"

// Setup the SYNC/BUSY pin to be either SYNC or BUSY, and to a desired
//  ticks per step level.
void dSPIN::configSyncPin(byte pinFunc, byte syncSteps, byte index)
{
  // Only some of the bits in this register are of interest to us; we need to
  //  clear those bits. It happens that they are the upper four.
  byte syncPinConfig = (byte)getParam(STEP_MODE, index);
  syncPinConfig &= 0x0F;
  
  // Now, let's OR in the arguments. We're going to mask the incoming
  //  data to avoid touching any bits that aren't appropriate. See datasheet
  //  for more info about which bits we're touching.
  syncPinConfig |= ((pinFunc & 0x80) | (syncSteps & 0x70));
  
  // Now we should be able to send that byte right back to the dSPIN- it
  //  won't corrupt the other bits, and the changes are made.
  setParam(STEP_MODE, (unsigned long)syncPinConfig, index);
}

void dSPIN::configSyncPin(byte pinFunc, byte syncSteps)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    configSyncPin(pinFunc, syncSteps, i);
  };
}

// The dSPIN chip supports microstepping for a smoother ride. This function
//  provides an easy front end for changing the microstepping mode.
void dSPIN::configStepMode(byte stepMode, byte index)
{

  // Only some of these bits are useful (the lower three). We'll extract the
  //  current contents, clear those three bits, then set them accordingly.
  byte stepModeConfig = (byte)getParam(STEP_MODE, index);
  stepModeConfig &= 0xF8;
  
  // Now we can OR in the new bit settings. Mask the argument so we don't
  //  accidentally the other bits, if the user sends us a non-legit value.
  stepModeConfig |= (stepMode&0x07);
  
  // Now push the change to the chip.
  setParam(STEP_MODE, (unsigned long)stepModeConfig, index);
}

void dSPIN::configStepMode(byte stepMode)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    configStepMode(stepMode, i);
  };
}

byte dSPIN::getStepMode(byte index) {
  return (byte)(getParam(STEP_MODE, index) & 0x07);
}

// This is the maximum speed the dSPIN will attempt to produce.
void dSPIN::setMaxSpeed(float stepsPerSecond, byte index)
{
  // We need to convert the floating point stepsPerSecond into a value that
  //  the dSPIN can understand. Fortunately, we have a function to do that.
  unsigned long integerSpeed = maxSpdCalc(stepsPerSecond);
  
  // Now, we can set that paramter.
  setParam(MAX_SPEED, integerSpeed, index);
}

void dSPIN::setMaxSpeed(float stepsPerSecond)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setMaxSpeed(stepsPerSecond, i);
  };
}

float dSPIN::getMaxSpeed(byte index)
{
  return maxSpdParse(getParam(MAX_SPEED, index));
}

// Set the minimum speed allowable in the system. This is the speed a motion
//  starts with; it will then ramp up to the designated speed or the max
//  speed, using the acceleration profile.
void dSPIN::setMinSpeed(float stepsPerSecond, byte index)
{
  // We need to convert the floating point stepsPerSecond into a value that
  //  the dSPIN can understand. Fortunately, we have a function to do that.
  unsigned long integerSpeed = minSpdCalc(stepsPerSecond);
  
  // MIN_SPEED also contains the LSPD_OPT flag, so we need to protect that.
  unsigned long temp = getParam(MIN_SPEED, index) & 0x00001000;
  
  // Now, we can set that paramter.
  setParam(MIN_SPEED, integerSpeed | temp, index);
}

void dSPIN::setMinSpeed(float stepsPerSecond)
{
  for (byte i=0; i<NDSPINS; i++) {
    setMinSpeed(stepsPerSecond, i);
  }
}

float dSPIN::getMinSpeed(byte index)
{
  return minSpdParse(getParam(MIN_SPEED, index));
}

// Above this threshold, the dSPIN will cease microstepping and go to full-step
//  mode. 
void dSPIN::setFullSpeed(float stepsPerSecond, byte index)
{
  unsigned long integerSpeed = FSCalc(stepsPerSecond);
  setParam(FS_SPD, integerSpeed, index);
}

void dSPIN::setFullSpeed(float stepsPerSecond)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setFullSpeed(stepsPerSecond, i);
  };
}

float dSPIN::getFullSpeed(byte index)
{
  return FSParse(getParam(FS_SPD, index));
}

// Set the acceleration rate, in steps per second per second. This value is
//  converted to a dSPIN friendly value. Any value larger than 29802 will
//  disable acceleration, putting the chip in "infinite" acceleration mode.
void dSPIN::setAcc(float stepsPerSecondPerSecond, byte index)
{
  unsigned long integerAcc = accCalc(stepsPerSecondPerSecond);
  setParam(ACC, integerAcc, index);
}

void dSPIN::setAcc(float stepsPerSecondPerSecond)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setAcc(stepsPerSecondPerSecond, i);
  };
}

float dSPIN::getAcc(byte index)
{
  return accParse(getParam(ACC, index));
}

// Same rules as setAcc().
void dSPIN::setDec(float stepsPerSecondPerSecond, byte index)
{
  unsigned long integerDec = decCalc(stepsPerSecondPerSecond);
  setParam(DECEL, integerDec, index);
}

void dSPIN::setDec(float stepsPerSecondPerSecond)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setDec(stepsPerSecondPerSecond, i);
  };
}

float dSPIN::getDec(byte index)
{
  return accParse(getParam(DECEL, index));
}

void dSPIN::setOCThreshold(byte threshold, byte index)
{
  setParam(OCD_TH, 0x0F & threshold, index);
}

void dSPIN::setOCThreshold(byte threshold)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setOCThreshold(threshold, i);
  };
}

byte dSPIN::getOCThreshold(byte index)
{
  return (byte) (getParam(OCD_TH, index) & 0xF);
}

void dSPIN::setStallThreshold(byte threshold, byte index)
{
  setParam(STALL_TH, 0x3F & threshold, index);
}

void dSPIN::setStallThreshold(byte threshold)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setStallThreshold(threshold, i);
  };
}

byte dSPIN::getStallThreshold(byte index)
{
  return (byte) (getParam(STALL_TH, index) & 0x3F);
}

// The next few functions are all breakouts for individual options within the
//  single register CONFIG. We'll read CONFIG, blank some bits, then OR in the
//  new value.

// This is a multiplier/divider setup for the PWM frequency when microstepping.
//  Divisors of 1-7 are available; multipliers of .625-2 are available. See
//  datasheet for more details; it's not clear what the frequency being
//  multiplied/divided here is, but it is clearly *not* the actual clock freq.
void dSPIN::setPWMFreq(int divisor, int multiplier, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  
  // The divisor is set by config 15:13, so mask 0xE000 to clear them.
  configVal &= ~(0xE000);
  // The multiplier is set by config 12:10; mask is 0x1C00
  configVal &= ~(0x1C00);
  // Now we can OR in the masked-out versions of the values passed in.
  configVal |= ((0xE000&divisor)|(0x1C00&multiplier));
  setParam(CONFIG, configVal, index);
}

void dSPIN::setPWMFreq(int divisor, int multiplier)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setPWMFreq(divisor, multiplier, i);
  }
}

int dSPIN::getPWMFreqDivisor(byte index)
{
  return (int) (getParam(CONFIG, index) & 0xE000);
}

int dSPIN::getPWMFreqMultiplier(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x1C00);
}

// Slew rate of the output in V/us. Can be 180, 290, or 530.
void dSPIN::setSlewRate(int slewRate, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  
  // These bits live in CONFIG 9:8, so the mask is 0x0300.
  configVal &= ~(0x0300);
  //Now, OR in the masked incoming value.
  configVal |= (0x0300&slewRate);
  setParam(CONFIG, configVal, index);
}

void dSPIN::setSlewRate(int slewRate)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setSlewRate(slewRate, i);
  }
}

int dSPIN::getSlewRate(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x0300);
}

// Single bit- do we shutdown the drivers on overcurrent or not?
void dSPIN::setOCShutdown(int OCShutdown, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  // This bit is CONFIG 7, mask is 0x0080
  configVal &= ~(0x0080);
  //Now, OR in the masked incoming value.
  configVal |= (0x0080 & OCShutdown);
  setParam(CONFIG, configVal, index);
}

void dSPIN::setOCShutdown(int OCShutdown)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setOCShutdown(OCShutdown, i);
  }
}

int dSPIN::getOCShutdown(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x0080);
}

// Enable motor voltage compensation? Not at all straightforward- check out
//  p34 of the datasheet.
void dSPIN::setVoltageComp(int vsCompMode, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  // This bit is CONFIG 5, mask is 0x0020
  configVal &= ~(0x0020);
  //Now, OR in the masked incoming value.
  configVal |= (0x0020 & vsCompMode);
  setParam(CONFIG, configVal, index);
}

void dSPIN::setVoltageComp(int vsCompMode)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setVoltageComp(vsCompMode, i);
  }
}

int dSPIN::getVoltageComp(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x0020);
}

// The switch input can either hard-stop the driver _or_ activate an interrupt.
//  This bit allows you to select what it does.
void dSPIN::setSwitchMode(int switchMode, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  // This bit is CONFIG 4, mask is 0x0010
  configVal &= ~(0x0010);
  //Now, OR in the masked incoming value.
  configVal |= (0x0010 & switchMode);
  setParam(CONFIG, configVal, index);
}

void dSPIN::setSwitchMode(int switchMode)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setSwitchMode(switchMode, i);
  }
}

int dSPIN::getSwitchMode(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x0010);
}

// There are a number of clock options for this chip- it can be configured to
//  accept a clock, drive a crystal or resonator, and pass or not pass the
//  clock signal downstream. Theoretically, you can use pretty much any
//  frequency you want to drive it; practically, this library assumes it's
//  being driven at 16MHz. Also, the device will use these bits to set the
//  math used to figure out steps per second and stuff like that.
void dSPIN::setOscMode(int oscillatorMode, byte index)
{
  unsigned long configVal = getParam(CONFIG, index);
  // These bits are CONFIG 3:0, mask is 0x000F
  configVal &= ~(0x000F);
  //Now, OR in the masked incoming value.
  configVal |= (0x000F & oscillatorMode);
  setParam(CONFIG, configVal, index);
}

void dSPIN::setOscMode(int oscillatorMode)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setOscMode(oscillatorMode, i);
  }
}

int dSPIN::getOscMode(byte index)
{
  return (int) (getParam(CONFIG, index) & 0x000F);
}

// The KVAL registers are...weird. I don't entirely understand how they differ
//  from the microstepping, but if you have trouble getting the motor to run,
//  tweaking KVAL has proven effective in the past. There's a separate register
//  for each case: running, static, accelerating, and decelerating.

void dSPIN::setAccKVAL(byte kvalInput, byte index)
{
  setParam(KVAL_ACC, kvalInput, index);
}

void dSPIN::setAccKVAL(byte kvalInput)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setAccKVAL(kvalInput, i);
  };
}

byte dSPIN::getAccKVAL(byte index)
{
  return (byte) getParam(KVAL_ACC, index);
}

void dSPIN::setDecKVAL(byte kvalInput, byte index)
{
  setParam(KVAL_DEC, kvalInput, index);
}

void dSPIN::setDecKVAL(byte kvalInput)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setDecKVAL(kvalInput, i);
  };
}

byte dSPIN::getDecKVAL(byte index)
{
  return (byte) getParam(KVAL_DEC, index);
}

void dSPIN::setRunKVAL(byte kvalInput, byte index)
{
  setParam(KVAL_RUN, kvalInput, index);
}

void dSPIN::setRunKVAL(byte kvalInput)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setRunKVAL(kvalInput, i);
  };
}

byte dSPIN::getRunKVAL(byte index)
{
  return (byte) getParam(KVAL_RUN, index);
}

void dSPIN::setHoldKVAL(byte kvalInput, byte index)
{
  setParam(KVAL_HOLD, kvalInput, index);
}

void dSPIN::setHoldKVAL(byte kvalInput)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setHoldKVAL(kvalInput, i);
  };
}

byte dSPIN::getHoldKVAL(byte index)
{
  return (byte) getParam(KVAL_HOLD, index);
}

// Enable or disable the low-speed optimization option. With LSPD_OPT enabled,
//  motion starts from 0 instead of MIN_SPEED and low-speed optimization keeps
//  the driving sine wave prettier than normal until MIN_SPEED is reached.
void dSPIN::setLoSpdOpt(boolean enable, byte index)
{
  unsigned long temp = getParam(MIN_SPEED, index);
  if (enable) temp |= 0x00001000; // Set the LSPD_OPT bit
  else        temp &= 0xffffefff; // Clear the LSPD_OPT bit
  setParam(MIN_SPEED, temp, index);
}

void dSPIN::setLoSpdOpt(boolean enable)
{
  for (byte i=0; i<NDSPINS; i++)
  {
    setLoSpdOpt(enable, i);
  }
}

boolean dSPIN::getLoSpdOpt(byte index)
{
  return (boolean) ((getParam(MIN_SPEED, index) & 0x00001000) != 0);
}
