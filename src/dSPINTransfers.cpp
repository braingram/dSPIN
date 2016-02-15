#include "dSPIN.h"
#include <SPI.h>

// used for SPI transfers
SPISettings settings(5000000, MSBFIRST, SPI_MODE3);

void dSPIN::lowerCS()
{
  #ifdef DSPIN_DEBUG
  Serial.print("cs=low\n");
  #endif
  //digitalWrite(_CSPin, LOW);
  *_cs_reg &= ~_cs_mask; 
  asm("NOP");  // give some time to setup
};

void dSPIN::raiseCS()
{
  #ifdef DSPIN_DEBUG
  Serial.print("cs=high\n");
  #endif
  //digitalWrite(_CSPin, HIGH);
  *_cs_reg |= _cs_mask; 
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
    // mask out GET_PARAM
    // 000xxxxx (set_param or noop)
    // 001xxxxx (get_param)
    /*
     * first check of (???xxxxx >> 5) > 1 [command vs param]
     * then check against ???????x >> 1 [command type]
    */
    if ((commands[i] >> 5) < 2) {  // param
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
    } else { // command
      byte cmd = (commands[i] & 0xFE);  // remove DIR bit
      switch (cmd)
      {
        case RUN:
        case MOVE:
        case GOTO:
        case GOTO_DIR:
        case GO_UNTIL:
        case 0x8A:  // GO_UNTIL + ACT
        case GET_STATUS:
          nBytes[i] = 3;
          break;
        case STEP_CLOCK:
        case RELEASE_SW:
        case 0x9A:  // RELEASE_SW + ACT
        case GO_HOME:
        case GO_MARK:
        case RESET_POS:
        case RESET_DEVICE:
        case SOFT_STOP:
        case HARD_STOP:
        case SOFT_HIZ:
        case HARD_HIZ:
          nBytes[i] = 0;
          break;
        default:
          nBytes[i] = 0;
          break;
      };
    };
 };
};

// Transfer currently queued commands
void dSPIN::transfer()
{
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
              Serial.print(i);
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
              #ifdef DSPIN_DEBUG
              Serial.print("--transfer(");
              Serial.print(i);
              Serial.print(")--\nb=0\nr=");
              Serial.print(ret, HEX);
              Serial.print('\n');
              #endif
          }
      };
      raiseCS();
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
