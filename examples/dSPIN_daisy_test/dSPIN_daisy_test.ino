#include <SPI.h>

#define CS 2
#define MOSI 11
#define MISO 12
#define SCK 13

#define RESET 4

#define NDSPINS 2

#define SET_PARAM 0x00
#define GET_PARAM 0x20
#define RESET_DEVICE 0xC0
#define GET_STATUS 0xD0

#define ABS_POS 0x01
#define CONFIG 0x18

//#define DEBUG 1

SPISettings settings(5000000, MSBFIRST, SPI_MODE3);

unsigned long values[NDSPINS];
byte commands[NDSPINS];
byte n_bytes[NDSPINS];


void setup_spi() {
    // setup pins
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    pinMode(MOSI, OUTPUT);
    pinMode(MISO, INPUT);
    pinMode(SCK, OUTPUT);
    pinMode(RESET, OUTPUT);
    // setup SPI
    SPI.begin();
    SPISettings settings(5000000, MSBFIRST, SPI_MODE3);
    // reset device
    digitalWrite(RESET, LOW);
    delay(5);
    digitalWrite(RESET, HIGH);
    delay(5);
}


void transfer() {
    SPI.beginTransaction(settings);

    byte ret = 0;
    byte max_n_bytes = 0;
    byte i = 0;
    byte offset = 0;
    #ifdef DEBUG
    Serial.print("cs=low\n");
    #endif
    digitalWrite(CS, LOW);
    // multiple transfers
    // 1) send commands
    for (i=0; i < NDSPINS; i++) {
        #ifdef DEBUG
        Serial.print("--transfer(command)(");
        Serial.print(i);
        Serial.print(")--\n");
        #endif
        ret = SPI.transfer(commands[i]);
        #ifdef DEBUG
        Serial.print("c=");
        Serial.print(commands[i], HEX);
        Serial.print("\nr=");
        Serial.print(ret, HEX);
        Serial.print('\n');
        #endif
        // find maximum number of bytes to send
        max_n_bytes = max(max_n_bytes, abs(n_bytes[i]));
    };
    #ifdef DEBUG
    Serial.print("cs=high\n");
    #endif
    digitalWrite(CS, HIGH);
    delayMicroseconds(1);

    // 2) send/receive values
    for (byte j=0; j < max_n_bytes; j++) {
        digitalWrite(CS, LOW);
        #ifdef DEBUG
        Serial.print("cs=low\n");
        #endif
        for (i=0; i < NDSPINS; i++) {
            if (n_bytes[i] != 0) {
                // byte offset to send/receive
                offset = n_bytes[i] - 1;
                // this command has either a value to send or receive
                // TODO one-line this?
                // extract byte to send
                byte* bp = (byte*)&(values[i]);
                // send byte
                ret = SPI.transfer(bp[offset]);
                #ifdef DEBUG
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
                n_bytes[i] = offset;
            } else {
                ret = SPI.transfer(0);
            }
        };
        #ifdef DEBUG
        Serial.print("cs=high\n");
        #endif
        digitalWrite(CS, HIGH);
        delayMicroseconds(1);
    };

    SPI.endTransaction();
};

void reset_buffers() {
    Serial.print("reset buffers\n");
    for (byte i=0; i < NDSPINS; i++) {
        values[i] = 0;
        n_bytes[i] = 0;
        commands[i] = 0;
    }
};


void setup() {
    // begin SPI communications
    Serial.begin(9600);
    Serial.print("starting spi...\n");
    setup_spi();

    reset_buffers();
}

void loop() {
    Serial.print("\n\n\n\n:::::Start of loop:::::\n");
    // get status from both autodrivers
    commands[0] = GET_STATUS;
    commands[1] = GET_STATUS;
    n_bytes[0] = 2;
    n_bytes[1] = 2;
    Serial.print(":STATUS:\n");
    transfer();
    // report status
    Serial.print("v0=");
    Serial.print(values[0], HEX);
    Serial.print("\nv1=");
    Serial.print(values[1], HEX);
    Serial.print("\n");
    reset_buffers();

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

    digitalWrite(RESET, LOW);
    delay(5);
    digitalWrite(RESET, HIGH);
    delay(5);
    
    // SetParam
    delay(5000);
}
