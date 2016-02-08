#include <SPI.h>

#define CS 10
#define MOSI 11
#define MISO 12
#define SCK 13

#define RESET 6

#define NDSPINS 2

#define SET_PARAM 0x00
#define GET_PARAM 0x20
#define RESET_DEVICE 0xC0
#define GET_STATUS 0xD0

#define ABS_POS 0x01

SPISettings settings(5000000, MSBFIRST, SPI_MODE3);

unsigned long values[NDSPINS];
byte commands[NDSPINS];
char n_bytes[NDSPINS];


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

    digitalWrite(CS, LOW);
    // multiple transfers
    // 1) send commands
    for (i=0; i < NDSPINS; i++) {
        ret = SPI.transfer(commands[i]);
        // find maximum number of bytes to send
        max_n_bytes = max(max_n_bytes, abs(n_bytes[i]));
    };
    digitalWrite(CS, HIGH);

    // 2) send/receive values
    for (byte j=0; j < max_n_bytes; j++) {
        digitalWrite(CS, LOW);
        for (i=0; i < NDSPINS; i++) {
            if (n_bytes[i] != 0) {
                // byte offset to send/receive
                offset = n_bytes - 1
                // this command has either a value to send or receive
                // TODO one-line this?
                // extract byte to send
                byte* bp = (byte*)&(values[i]);
                // send byte
                ret = SPI.transfer(byte[offset]);
                // save result
                byte[offset] = ret;
                n_bytes[i] = offset;
            };
        };
        digitalWrite(CS, HIGH);
    };

    SPI.endTransation();
}

void reset_buffers() {
    Serial.write("reset buffers\n")
    for (byte i=0; i < NDSPINS; i++) {
        values[i] = 0;
        n_bytes[i] = 0;
        commands[i] = 0;
    }
}


void setup() {
    // begin SPI communications
    Serial.begin(9600);
    Serial.write("starting spi...\n")
    setup_spi();

    reset_buffers();
}

void loop() {
    // get status from both autodrivers
    commands[0] = GET_STATUS;
    commands[1] = GET_STATUS;
    n_bytes[0] = 2;
    n_bytes[1] = 2;
    transfer();
    // report status
    Serial.write(":STATUS:\n")
    Serial.write(values[0]);
    Serial.write("\n");
    Serial.write(values[1]);
    Serial.write("\n");
    reset_buffers();

    // get_param abs_pos
    commands[0] = GET_PARAM | ABS_POS;
    commands[1] = GET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    transfer();
    // report abs_pos
    Serial.write(":GET_PARAM | ABS_POS:\n")
    Serial.write(values[0]);
    Serial.write("\n");
    Serial.write(values[1]);
    Serial.write("\n");
    reset_buffers();

    // set_param abs_pos
    commands[0] = SET_PARAM | ABS_POS;
    commands[1] = SET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    values[0] = 123456;
    values[1] = 789012;
    transfer();
    Serial.write(":SET_PARAM | ABS_POS:\n")
    reset_buffers();

     // get_param abs_pos
    commands[0] = GET_PARAM | ABS_POS;
    commands[1] = GET_PARAM | ABS_POS;
    n_bytes[0] = 3;
    n_bytes[1] = 3;
    transfer();
    // report abs_pos
    Serial.write(":GET_PARAM | ABS_POS:\n")
    Serial.write(values[0]);
    Serial.write("\n");
    Serial.write(values[1]);
    Serial.write("\n");
    reset_buffers();

    // reset device
    commands[0] = RESET_DEVICE;
    commands[1] = RESET_DEVICE;
    transfer();
    // report status
    Serial.write(":RESET_DEVICE:\n")
    reset_buffers();

    // SetParam
    delay(5000);
}
