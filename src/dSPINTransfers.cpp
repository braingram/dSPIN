#include "dSPIN.h"
#include <SPI.h>

// used for SPI transfers
SPISettings settings(5000000, MSBFIRST, SPI_MODE3);

void dSPIN::lowerCS()
{
  #ifdef DSPIN_DEBUG
  Serial.print("cs=low\n");
  #endif
  // TODO speedup
  digitalWrite(_CSPin, LOW);
};

void dSPIN::raiseCS()
{
  #ifdef DSPIN_DEBUG
  Serial.print("cs=high\n");
  #endif
  // TODO speedup
  digitalWrite(_CSPin, HIGH);
};

void dSPIN::setCommand(byte command, byte index)
{
  commands[index] = command;
};

void dSPIN::setCommand(byte command)
{
  for (byte i=0; i<NDSPINS; i++) {
    setCommand(command, i);
  };
};

void dSPIN::setValue(unsigned long value, byte index)
{
  values[index] = value;
};

void dSPIN::setValue(unsigned long value)
{
  for (byte i=0; i<NDSPINS; i++) {
    setValue(value, i);
  };
};

void dSPIN::setValueByte(byte offset, byte value, byte index)
{
  byte* bp = (byte*)&(values[index]);
  bp[offset] = value;
};

void dSPIN::setValueByte(byte offset, byte value)
{
  for (byte i=0; i<NDSPINS; i++) {
    setValueByte(offset, value, i);
  };
};

void dSPIN::setNBytes(byte n, byte index)
{
  nBytes[index] = n;
};

void dSPIN::setNBytes(byte n)
{
  for (byte i=0; i<NDSPINS; i++) {
    setNBytes(n, i);
  };
};

void dSPIN::setNBytes()
{
  for (byte i=0; i<NDSPINS; i++) {
    // TODO make this compatible with other commands
    // mask out GET_PARAM
    // 000xxxxx (set_param or noop)
    // 001xxxxx (get_param)
    switch (commands[i] >> 5)
    switch (commands[i] & 0x1F)
    {
      case ABS_POS:
      case MARK:
      case SPEED:
        nBytes[i] = 3;
        break;
      case EL_POS:
      case ACC:
      case DECEL:
      case MAX_SPEED:
      case MIN_SPEED:
      case FS_SPD:
      case INT_SPD:
      case CONFIG:
      case STATUS:
        nBytes[i] = 2;
        break;
      case KVAL_HOLD:
      case KVAL_RUN:
      case KVAL_ACC:
      case KVAL_DEC:
      case ST_SLP:
      case FN_SLP_ACC:
      case FN_SLP_DEC:
      case K_THERM:
      case ADC_OUT:
      case STALL_TH:
      case STEP_MODE:
      case ALARM_EN:
        nBytes[i] = 1;
        break;
      default:
        nBytes[i] = 0;
        break;
    };
 };
};

// Transfer currently queued commands
void dSPIN::transfer()
{
  // TODO
  SPI.beginTransaction(settings);

  byte ret = 0;
  byte maxNBytes = 0;
  byte i = 0;
  byte offset = 0;
  lowerCS();
  // multiple transfers
  // 1) send commands
  for (i=0; i < NDSPINS; i++) {
      #ifdef DSPIN_DEBUG
      Serial.print("--transfer(command)(");
      Serial.print(i);
      Serial.print(")--\n");
      #endif
      ret = SPI.transfer(commands[i]);
      #ifdef DSPIN_DEBUG
      Serial.print("c=");
      Serial.print(commands[i], HEX);
      Serial.print("\nr=");
      Serial.print(ret, HEX);
      Serial.print('\n');
      #endif
      // find maximum number of bytes to send
      maxNBytes = max(maxNBytes, abs(nBytes[i]));
  };
  raiseCS();
  delayMicroseconds(1);  // TODO necessary?

  // 2) send/receive values
  for (byte j=0; j < maxNBytes; j++) {
      lowerCS();
      for (i=0; i < NDSPINS; i++) {
          if (nBytes[i] != 0) {
              // byte offset to send/receive
              offset = nBytes[i] - 1;
              // this command has either a value to send or receive
              // TODO one-line this?
              // extract byte to send
              byte* bp = (byte*)&(values[i]);
              // send byte
              ret = SPI.transfer(bp[offset]);
              #ifdef DSPIN_DEBUG
              Serial.print("--transfer(");
              Serial.print(j);
              Serial.print(")--\nv=");
              Serial.print(values[i], HEX);
              Serial.print("\no=");
              Serial.print(offset, HEX);
              Serial.print("\nb=");
              Serial.print(bp[offset], HEX);
              Serial.print("\nr=");
              Serial.print(ret, HEX);
              Serial.print('\n');
              #endif
              // save result
              bp[offset] = ret;
              nBytes[i] = offset;
          } else {
              ret = SPI.transfer(0);
          }
      };
      raiseCS();
      delayMicroseconds(1);  // TODO necessary?
  };

  SPI.endTransaction();
};

unsigned long dSPIN::getValue(byte index)
{
  return values[index];
};

unsigned long dSPIN::getValue()
{
  return getValue(0);
};

// clear buffers after a transfer, will overwrite any results
void dSPIN::resetBuffers()
{
  for (byte i=0; i<NDSPINS; i++) {
    values[i] = 0;
    commands[i] = 0;
    nBytes[i] = 0;
  };
};

void dSPIN::runCommand(byte command, unsigned long value, byte index)
{
  setCommand(command, index);
  setValue(value, index);
  setNBytes();
  transfer();
  resetBuffers();
};

void dSPIN::runCommand(byte command, unsigned long value)
{
  setCommand(command);
  setValue(value);
  setNBytes();
  transfer();
  resetBuffers();
};

void dSPIN::runCommand(byte command, byte index)
{
  setCommand(command, index);
  setNBytes();
  transfer();
  resetBuffers();
};

void dSPIN::runCommand(byte command)
{
  setCommand(command);
  setNBytes();
  transfer();
  resetBuffers();
};

void dSPIN::setParam(byte param, unsigned long value, byte index)
{
  runCommand(SET_PARAM | param, value, index);
};

void dSPIN::setParam(byte param, unsigned long value)
{
  runCommand(SET_PARAM | param, value);
};

unsigned long dSPIN::getParam(byte param, byte index)
{
  setCommand(GET_PARAM | param, index);
  setNBytes();
  transfer();
  unsigned long retVal = getValue(index);
  resetBuffers();
  return retVal;
};

/*
// Much of the functionality between "get parameter" and "set parameter" is
//  very similar, so we deal with that by putting all of it in one function
//  here to save memory space and simplify the program.
unsigned long dSPIN::paramHandler(byte param, unsigned long value)
{
  unsigned long retVal = 0;   // This is a temp for the value to return.
  
  // This switch structure handles the appropriate action for each register.
  //  This is necessary since not all registers are of the same length, either
  //  bit-wise or byte-wise, so we want to make sure we mask out any spurious
  //  bits and do the right number of transfers. That is handled by the xferParam()
  //  function, in most cases, but for 1-byte or smaller transfers, we call
  //  SPIXfer() directly.
  switch (param)
  {
    // ABS_POS is the current absolute offset from home. It is a 22 bit number expressed
    //  in two's complement. At power up, this value is 0. It cannot be written when
    //  the motor is running, but at any other time, it can be updated to change the
    //  interpreted position of the motor.
    case ABS_POS:
      retVal = xferParam(value, 22);
      break;
    // EL_POS is the current electrical position in the step generation cycle. It can
    //  be set when the motor is not in motion. Value is 0 on power up.
    case EL_POS:
      retVal = xferParam(value, 9);
      break;
    // MARK is a second position other than 0 that the motor can be told to go to. As
    //  with ABS_POS, it is 22-bit two's complement. Value is 0 on power up.
    case MARK:
      retVal = xferParam(value, 22);
      break;
    // SPEED contains information about the current speed. It is read-only. It does 
    //  NOT provide direction information.
    case SPEED:
      retVal = xferParam(0, 20);
      break; 
    // ACC and DEC set the acceleration and deceleration rates. Set ACC to 0xFFF 
    //  to get infinite acceleration/decelaeration- there is no way to get infinite
    //  deceleration w/o infinite acceleration (except the HARD STOP command).
    //  Cannot be written while motor is running. Both default to 0x08A on power up.
    // AccCalc() and DecCalc() functions exist to convert steps/s/s values into
    //  12-bit values for these two registers.
    case ACC: 
      retVal = xferParam(value, 12);
      break;
    case DECEL: 
      retVal = xferParam(value, 12);
      break;
    // MAX_SPEED is just what it says- any command which attempts to set the speed
    //  of the motor above this value will simply cause the motor to turn at this
    //  speed. Value is 0x041 on power up.
    // MaxSpdCalc() function exists to convert steps/s value into a 10-bit value
    //  for this register.
    case MAX_SPEED:
      retVal = xferParam(value, 10);
      break;
    // MIN_SPEED controls two things- the activation of the low-speed optimization
    //  feature and the lowest speed the motor will be allowed to operate at. LSPD_OPT
    //  is the 13th bit, and when it is set, the minimum allowed speed is automatically
    //  set to zero. This value is 0 on startup.
    // MinSpdCalc() function exists to convert steps/s value into a 12-bit value for this
    //  register. SetLSPDOpt() function exists to enable/disable the optimization feature.
    case MIN_SPEED: 
      retVal = xferParam(value, 13);
      break;
    // FS_SPD register contains a threshold value above which microstepping is disabled
    //  and the dSPIN operates in full-step mode. Defaults to 0x027 on power up.
    // FSCalc() function exists to convert steps/s value into 10-bit integer for this
    //  register.
    case FS_SPD:
      retVal = xferParam(value, 10);
      break;
    // KVAL is the maximum voltage of the PWM outputs. These 8-bit values are ratiometric
    //  representations: 255 for full output voltage, 128 for half, etc. Default is 0x29.
    // The implications of different KVAL settings is too complex to dig into here, but
    //  it will usually work to max the value for RUN, ACC, and DEC. Maxing the value for
    //  HOLD may result in excessive power dissipation when the motor is not running.
    case KVAL_HOLD:
      retVal = SPIXfer((byte)value);
      break;
    case KVAL_RUN:
      retVal = SPIXfer((byte)value);
      break;
    case KVAL_ACC:
      retVal = SPIXfer((byte)value);
      break;
    case KVAL_DEC:
      retVal = SPIXfer((byte)value);
      break;
    // INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back EMF
    //  compensation functionality. Please see the datasheet for details of this
    //  function- it is too complex to discuss here. Default values seem to work
    //  well enough.
    case INT_SPD:
      retVal = xferParam(value, 14);
      break;
    case ST_SLP: 
      retVal = SPIXfer((byte)value);
      break;
    case FN_SLP_ACC: 
      retVal = SPIXfer((byte)value);
      break;
    case FN_SLP_DEC: 
      retVal = SPIXfer((byte)value);
      break;
    // K_THERM is motor winding thermal drift compensation. Please see the datasheet
    //  for full details on operation- the default value should be okay for most users.
    case K_THERM: 
      value &= 0x0F;
      retVal = SPIXfer((byte)value);
      break;
    // ADC_OUT is a read-only register containing the result of the ADC measurements.
    //  This is less useful than it sounds; see the datasheet for more information.
    case ADC_OUT:
      retVal = SPIXfer((byte)value);
      break;
    // Set the overcurrent threshold. Ranges from 375mA to 6A in steps of 375mA.
    //  A set of defined constants is provided for the user's convenience. Default
    //  value is 3.375A- 0x08. This is a 4-bit value.
    case OCD_TH: 
      value &= 0x0F;
      retVal = SPIXfer((byte)value);
      break;
    // Stall current threshold. Defaults to 0x40, or 2.03A. Value is from 31.25mA to
    //  4A in 31.25mA steps. This is a 7-bit value.
    case STALL_TH: 
      value &= 0x7F;
      retVal = SPIXfer((byte)value);
      break;
    // STEP_MODE controls the microstepping settings, as well as the generation of an
    //  output signal from the dSPIN. Bits 2:0 control the number of microsteps per
    //  step the part will generate. Bit 7 controls whether the BUSY/SYNC pin outputs
    //  a BUSY signal or a step synchronization signal. Bits 6:4 control the frequency
    //  of the output signal relative to the full-step frequency; see datasheet for
    //  that relationship as it is too complex to reproduce here.
    // Most likely, only the microsteps per step value will be needed; there is a set
    //  of constants provided for ease of use of these values.
    case STEP_MODE:
      retVal = SPIXfer((byte)value);
      break;
    // ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of constants
    //  is provided to make this easy to interpret. By default, ALL alarms will trigger the
    //  FLAG pin.
    case ALARM_EN: 
      retVal = SPIXfer((byte)value);
      break;
    // CONFIG contains some assorted configuration bits and fields. A fairly comprehensive
    //  set of reasonably self-explanatory constants is provided, but users should refer
    //  to the datasheet before modifying the contents of this register to be certain they
    //  understand the implications of their modifications. Value on boot is 0x2E88; this
    //  can be a useful way to verify proper start up and operation of the dSPIN chip.
    case CONFIG: 
      retVal = xferParam(value, 16);
      break;
    // STATUS contains read-only information about the current condition of the chip. A
    //  comprehensive set of constants for masking and testing this register is provided, but
    //  users should refer to the datasheet to ensure that they fully understand each one of
    //  the bits in the register.
    case STATUS:  // STATUS is a read-only register
      retVal = xferParam(0, 16);;
      break;
    default:
      retVal = SPIXfer((byte)value);
      break;
  }
  return retVal;
}

// Generalization of the subsections of the register read/write functionality.
//  We want the end user to just write the value without worrying about length,
//  so we pass a bit length parameter from the calling function.
unsigned long dSPIN::xferParam(unsigned long value, byte bitLen)
{
  byte byteLen = bitLen/8;      // How many BYTES do we have?
  if (bitLen%8 > 0) byteLen++;  // Make sure not to lose any partial byte values.
  // Let's make sure our value has no spurious bits set, and if the value was too
  //  high, max it out.
  unsigned long mask = 0xffffffff >> (32-bitLen);
  if (value > mask) value = mask;
  
  byte* bytePointer = (byte*)&value;
  for (signed char i = byteLen-1; i >= 0; i--)
  {
    bytePointer[i] = SPIXfer(bytePointer[i]);
  }

  return value;
}

byte dSPIN::SPIXfer(byte data)
{
  byte rxData;
  SPI.beginTransaction(settings);
  digitalWrite(_CSPin, LOW);
  rxData = SPI.transfer(data); 
  digitalWrite(_CSPin, HIGH);
  SPI.endTransaction();
  return rxData;
}
*/
