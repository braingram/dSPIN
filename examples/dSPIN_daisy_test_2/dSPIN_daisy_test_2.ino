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
    dspin.setCommand(0, GET_STATUS);
    dspin.setCommand(1, GET_STATUS);
    dspin.setNBytes(0, 2);
    dspin.setNBytes(1, 2);
    Serial.print(":STATUS:\n");
    dspin.transfer();
    Serial.print("v0=");
    Serial.print(dspin.getValue(0), HEX);
    Serial.print("\nv1=");
    Serial.print(dspin.getValue(1), HEX);
    Serial.print("\n");
    dspin.resetBuffers();

    /*
    // get_param config
    commands[0] = GET_PARAM | CONFIG;
    commands[1] = GET_PARAM | CONFIG;
    n_bytes[0] = 2;
    n_bytes[1] = 2;
    Serial.print(":GET_PARAM | CONFIG:\n");
    transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(values[0], HEX);
    Serial.print("\nv1=");
    Serial.print(values[1], HEX);
    Serial.print("\n");
    reset_buffers();

    // get_param abs_pos
    commands[0] = GET_PARAM | ABS_POS;
    commands[1] = GET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    Serial.print(":GET_PARAM | ABS_POS:\n");
    transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(values[0], HEX);
    Serial.print("\nv1=");
    Serial.print(values[1], HEX);
    Serial.print("\n");
    reset_buffers();

    // set_param abs_pos
    commands[0] = SET_PARAM | ABS_POS;
    commands[1] = SET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    values[0] = 123456;
    values[1] = 789012;
    Serial.print(":SET_PARAM | ABS_POS:\n");
    Serial.print("v0=");
    Serial.print(values[0], HEX);
    Serial.print("\nv1=");
    Serial.print(values[1], HEX);
    Serial.print("\n");
    transfer();
    reset_buffers();

     // get_param abs_pos
    commands[0] = GET_PARAM | ABS_POS;
    commands[1] = GET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    Serial.print(":GET_PARAM | ABS_POS:\n");
    transfer();
    // report abs_pos
    Serial.print("v0=");
    Serial.print(values[0], HEX);
    Serial.print("\nv1=");
    Serial.print(values[1], HEX);
    Serial.print("\n");
    reset_buffers();

    // reset device
    commands[0] = RESET_DEVICE;
    commands[1] = RESET_DEVICE;
    Serial.print(":RESET_DEVICE:\n");
    transfer();
    // report status
    reset_buffers();
*/
    digitalWrite(RESET, LOW);
    delay(5);
    digitalWrite(RESET, HIGH);
    delay(5);
    
    // SetParam
    delay(5000);
}
