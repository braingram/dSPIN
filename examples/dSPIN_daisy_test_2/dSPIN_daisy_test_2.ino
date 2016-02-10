#include <dSPIN.h>

#define CS 2
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


void setup() {
    // begin SPI communications
    Serial.begin(9600);
    Serial.print("starting spi...\n");
}

void loop() {
    Serial.print("\n\n\n\n:::::Start of loop:::::\n");
    dspin.setCommand(GET_STATUS, 0);
    dspin.setCommand(GET_STATUS, 1);
    dspin.setNBytes(2, 0);
    dspin.setNBytes(2, 1);
    Serial.print(":STATUS:\n");
    dspin.transfer();
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.resetBuffers();

    // get_param config
    dspin.setCommand(GET_PARAM | CONFIG);
    dspin.setNBytes(2);
    Serial.print(":GET_PARAM | CONFIG:\n");
    dspin.transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.resetBuffers();

    // get_param abs_pos
    dspin.setCommand(GET_PARAM | ABS_POS);
    dspin.setNBytes(3);
    Serial.print(":GET_PARAM | ABS_POS:\n");
    dspin.transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.resetBuffers();

    // set_param abs_pos
    dspin.setCommand(SET_PARAM | ABS_POS);
    dspin.setNBytes(3);
    dspin.setValue(123456, 0);
    dspin.setValue(789012, 1);
    Serial.print(":SET_PARAM | ABS_POS:\n");
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.transfer();
    dspin.resetBuffers();

    // get_param abs_pos
    dspin.setCommand(GET_PARAM | ABS_POS);
    dspin.setNBytes(3);
    Serial.print(":GET_PARAM | ABS_POS:\n");
    dspin.transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.resetBuffers();

    // reset device
    dspin.setCommand(RESET_DEVICE);
    Serial.print(":RESET_DEVICE:\n");
    dspin.transfer();
    // report status
    dspin.resetBuffers();

    digitalWrite(RESET, LOW);
    delay(5);
    digitalWrite(RESET, HIGH);
    delay(5);
    
    // SetParam
    delay(5000);
}
