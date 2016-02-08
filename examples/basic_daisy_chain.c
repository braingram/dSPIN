#include <SPI.h>

#define CS 10
#define MOSI 11
#define MISO 12
#define SCK 13

#define RESET 6

#define NDSPINS 2

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
    for (byte i=0; i < NDSPINS; i++) {
        values[i] = 0;
        n_bytes[i] = 0;
        commands[i] = 0;
    }
}


void setup() {
    // begin SPI communications
    setup_spi();

    reset_buffers();
}

void loop() {
    //
}
