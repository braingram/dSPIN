#include "dSPIN.h"

// Returns the content of the ABS_POS register, which is a signed 22-bit number
//  indicating the number of steps the motor has traveled from the HOME
//  position. HOME is defined by zeroing this register, and it is zero on
//  startup.
long dSPIN::getPos(byte index)
{
  long temp = getParam(ABS_POS, index);
  // Since ABS_POS is a 22-bit 2's comp value, we need to check bit 21 and, if
  //  it's set, set all the bits ABOVE 21 in order for the value to maintain
  //  its appropriate sign.
  if (temp & 0x00200000) temp |= 0xffC00000;
  return temp;
};


// Just like getPos(), but for MARK.
long dSPIN::getMark(byte index)
{
  long temp = getParam(MARK, index);
  
  // Since ABS_POS is a 22-bit 2's comp value, we need to check bit 21 and, if
  //  it's set, set all the bits ABOVE 21 in order for the value to maintain
  //  its appropriate sign.
  if (temp & 0x00200000) temp |= 0xffC00000;
  return temp;
}

// RUN sets the motor spinning in a direction (defined by the constants
//  FWD and REV). Maximum speed and minimum speed are defined
//  by the MAX_SPEED and MIN_SPEED registers; exceeding the FS_SPD value
//  will switch the device into full-step mode.
// The spdCalc() function is provided to convert steps/s values into
//  appropriate integer values for this function.
void dSPIN::run(byte dir, float stepsPerSec, byte index)
{
  unsigned long integerSpeed = spdCalc(stepsPerSec);
  if (integerSpeed > 0xFFFFF) integerSpeed = 0xFFFFF;
  setCommand(RUN | dir, index);
  setNBytes(2, index);
  setValue(integerSpeed, index);
  transfer();
  resetBuffers();
}

void dSPIN::run(byte dir, float stepsPerSec)
{
  unsigned long integerSpeed = spdCalc(stepsPerSec);
  if (integerSpeed > 0xFFFFF) integerSpeed = 0xFFFFF;
  setCommand(RUN | dir);
  setNBytes(2);
  setValue(integerSpeed);
  transfer();
  resetBuffers();
}

// STEP_CLOCK puts the device in external step clocking mode. When active,
//  pin 25, STCK, becomes the step clock for the device, and steps it in
//  the direction (set by the FWD and REV constants) imposed by the call
//  of this function. Motion commands (RUN, MOVE, etc) will cause the device
//  to exit step clocking mode.
void dSPIN::stepClock(byte dir, byte index)
{
  runCommand(STEP_CLOCK | dir, index);
}

void dSPIN::stepClock(byte dir)
{
  runCommand(STEP_CLOCK | dir);
}

// MOVE will send the motor numStep full steps in the
//  direction imposed by dir (FWD or REV constants may be used). The motor
//  will accelerate according the acceleration and deceleration curves, and
//  will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well.
void dSPIN::move(byte dir, unsigned long numSteps, byte index)
{
  if (numSteps > 0x3FFFFF) numSteps = 0x3FFFFF;
  setCommand(MOVE | dir, index);
  setNBytes(2, index);
  setValue(numSteps, index);
  transfer();
  resetBuffers();
}

void dSPIN::move(byte dir, unsigned long numSteps)
{
  if (numSteps > 0x3FFFFF) numSteps = 0x3FFFFF;
  setCommand(MOVE | dir);
  setNBytes(2);
  setValue(numSteps);
  transfer();
  resetBuffers();
}

// GOTO operates much like MOVE, except it produces absolute motion instead
//  of relative motion. The motor will be moved to the indicated position
//  in the shortest possible fashion.
void dSPIN::goTo(long pos, byte index)
{
  if (pos > 0x3FFFFF) pos = 0x3FFFFF;
  setCommand(GOTO, index);
  setNBytes(2, index);
  setValue(pos, index);
  transfer();
  resetBuffers();
}

void dSPIN::goTo(long pos)
{
  if (pos > 0x3FFFFF) pos = 0x3FFFFF;
  setCommand(GOTO);
  setNBytes(2);
  setValue(pos);
  transfer();
  resetBuffers();
}

// Same as GOTO, but with user constrained rotational direction.
void dSPIN::goToDir(byte dir, long pos, byte index)
{
  if (pos > 0x3FFFFF) pos = 0x3FFFFF;
  setCommand(GOTO_DIR | dir, index);
  setNBytes(2, index);
  setValue(pos, index);
  transfer();
  resetBuffers();
}

void dSPIN::goToDir(byte dir, long pos)
{
  if (pos > 0x3FFFFF) pos = 0x3FFFFF;
  setCommand(GOTO_DIR | dir);
  setNBytes(2);
  setValue(pos);
  transfer();
  resetBuffers();
}

// GoUntil will set the motor running with direction dir (REV or
//  FWD) until a falling edge is detected on the SW pin. Depending
//  on bit SW_MODE in CONFIG, either a hard stop or a soft stop is
//  performed at the falling edge, and depending on the value of
//  act (either RESET or COPY) the value in the ABS_POS register is
//  either RESET to 0 or COPY-ed into the MARK register.
void dSPIN::goUntil(byte action, byte dir, float stepsPerSec, byte index)
{
  unsigned long integerSpeed = spdCalc(stepsPerSec);
  if (integerSpeed > 0x3FFFFF) integerSpeed = 0x3FFFFF;
  setCommand(GO_UNTIL | dir, index);
  setNBytes(2, index);
  setValue(integerSpeed, index);
  transfer();
  resetBuffers();
}

void dSPIN::goUntil(byte action, byte dir, float stepsPerSec)
{
  unsigned long integerSpeed = spdCalc(stepsPerSec);
  if (integerSpeed > 0x3FFFFF) integerSpeed = 0x3FFFFF;
  setCommand(GO_UNTIL | dir);
  setNBytes(2);
  setValue(integerSpeed);
  transfer();
  resetBuffers();
}

// Similar in nature to GoUntil, ReleaseSW produces motion at the
//  higher of two speeds: the value in MIN_SPEED or 5 steps/s.
//  The motor continues to run at this speed until a rising edge
//  is detected on the switch input, then a hard stop is performed
//  and the ABS_POS register is either COPY-ed into MARK or RESET to
//  0, depending on whether RESET or COPY was passed to the function
//  for act.
void dSPIN::releaseSw(byte action, byte dir, byte index)
{
  runCommand(RELEASE_SW | action | dir, index);
}

void dSPIN::releaseSw(byte action, byte dir)
{
  runCommand(RELEASE_SW | action | dir);
}

// GoHome is equivalent to GoTo(0), but requires less time to send.
//  Note that no direction is provided; motion occurs through shortest
//  path. If a direction is required, use GoTo_DIR().
void dSPIN::goHome(byte index)
{
  runCommand(GO_HOME, index);
}

void dSPIN::goHome()
{
  runCommand(GO_HOME);
}

// GoMark is equivalent to GoTo(MARK), but requires less time to send.
//  Note that no direction is provided; motion occurs through shortest
//  path. If a direction is required, use GoTo_DIR().
void dSPIN::goMark(byte index)
{
  runCommand(GO_MARK, index);
}

void dSPIN::goMark()
{
  runCommand(GO_MARK);
}

// setMark() and setHome() allow the user to define new MARK or
//  ABS_POS values.
void dSPIN::setMark(long newMark, byte index)
{
  setParam(MARK, newMark, index);
}

void dSPIN::setMark(long newMark)
{
  setParam(MARK, newMark);
}

void dSPIN::setPos(long newPos, byte index)
{
  setParam(ABS_POS, newPos, index);
}

void dSPIN::setPos(long newPos)
{
  setParam(ABS_POS, newPos);
}

// Sets the ABS_POS register to 0, effectively declaring the current
//  position to be "HOME".
void dSPIN::resetPos(byte index)
{
  runCommand(RESET_POS, index);
}

void dSPIN::resetPos()
{
  runCommand(RESET_POS);
}

// Reset device to power up conditions. Equivalent to toggling the STBY
//  pin or cycling power.
void dSPIN::resetDev(byte index)
{
  runCommand(RESET_DEVICE, index);
}

void dSPIN::resetDev()
{
  runCommand(RESET_DEVICE);
}
  
// Bring the motor to a halt using the deceleration curve.
void dSPIN::softStop(byte index)
{
  runCommand(SOFT_STOP, index);
}

void dSPIN::softStop()
{
  runCommand(SOFT_STOP);
}

// Stop the motor with infinite deceleration.
void dSPIN::hardStop(byte index)
{
  runCommand(HARD_STOP, index);
}

void dSPIN::hardStop()
{
  runCommand(HARD_STOP);
}

// Decelerate the motor and put the bridges in Hi-Z state.
void dSPIN::softHiZ(byte index)
{
  runCommand(SOFT_HIZ, index);
}

void dSPIN::softHiZ()
{
  runCommand(SOFT_HIZ);
}

// Put the bridges in Hi-Z state immediately with no deceleration.
void dSPIN::hardHiZ(byte index)
{
  runCommand(HARD_HIZ, index);
}

void dSPIN::hardHiZ()
{
  runCommand(HARD_HIZ);
}

/*
// Fetch and return the 16-bit value in the STATUS register. Resets
//  any warning flags and exits any error states. Using GetParam()
//  to read STATUS does not clear these values.
int dSPIN::getStatus()
{
  int temp = 0;
  byte* bytePointer = (byte*)&temp;
  SPIXfer(GET_STATUS);
  bytePointer[1] = SPIXfer(0);
  bytePointer[0] = SPIXfer(0);
  return temp;
}
*/
