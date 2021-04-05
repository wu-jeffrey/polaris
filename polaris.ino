#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <SPI.h>
#include <Vector.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include "color.cpp"

#define PIN 6
#define N_LEDS 24
#define N_LEDS_PER_STICK 8

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_RGBW + NEO_KHZ800);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

enum Directions { STOP = 0,
                  FORWARD = 1,
                  LEFT = 2,
                  RIGHT = 3,
};

// Settings that can be changed via BlueTooth ////////////////////////////////////////////
Color currentColor = basicColors[WHITE];

int brightness = 10;

int _forwardPixels[N_LEDS_PER_STICK];
Vector<int> forwardPixels;

int _rightPixels[N_LEDS_PER_STICK];
Vector<int> rightPixels;

int _leftPixels[N_LEDS_PER_STICK];
Vector<int> leftPixels;

/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    ledStrip.begin();
    Serial.begin(115200);

    forwardPixels.setStorage(_forwardPixels);
    forwardPixels.push_back(11);
    forwardPixels.push_back(12);

    rightPixels.setStorage(_rightPixels);
    rightPixels.push_back(0);
    rightPixels.push_back(1);

    leftPixels.setStorage(_leftPixels);
    leftPixels.push_back(23);
    leftPixels.push_back(22);

    // bluetooth setup
    /* Initialise the module */
    Serial.print(F("Initialising the Bluefruit LE module: "));

    if (!ble.begin(VERBOSE_MODE)) {
        error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
    }
    Serial.println(F("OK!"));

    /* Disable command echo from Bluefruit */
    ble.echo(false);

    Serial.println("Requesting Bluefruit info:");
    /* Print Bluefruit information */
    ble.info();

    Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
    Serial.println(F("Then Enter characters to send to Bluefruit"));
    Serial.println();

    ble.verbose(false);  // debug info is a little annoying after this point!

    /* Wait for connection */
    while (!ble.isConnected()) {
        delay(500);
    }
}

void loop() {
    ledStrip.clear();
    ledStrip.setBrightness(brightness);

    switch (pollInstruction()) {
        case STOP:
            ledStrip.clear();
            ledStrip.show();
            break;
        case FORWARD:
            turnOnPixels(forwardPixels);
            break;
        case RIGHT:
            turnOnPixels(rightPixels);
            break;
        case LEFT:
            turnOnPixels(leftPixels);
            break;
    }
}

enum Buttons {
    BUTTON_FORWARD = 5,
    BUTTON_RIGHT = 7,
    BUTTON_LEFT = 8,
    BUTTON_BRIGHTNESS_UP = 1,
    BUTTON_BRIGHTNESS_DOWN = 3,
};

int pollInstruction() {
    // Check for incoming characters from Bluefruit
    ble.println("AT+BLEUARTRX");
    ble.readline();
    if (strcmp(ble.buffer, "OK") == 0) {
        // no data
        return;
    }
    // Some data was found, its in the buffer
    String buffer = String(ble.buffer);

    enum Directions d = STOP;

    // D-pad Controller Input
    if (buffer[1] == 'B') {
        uint8_t btnNum = buffer[2] - '0';
        boolean pressed = buffer[3] - '0';

        if (pressed) {
            switch (int(btnNum)) {
                case BUTTON_FORWARD:
                    d = FORWARD;
                    break;
                case BUTTON_RIGHT:
                    d = RIGHT;
                    break;
                case BUTTON_LEFT:
                    d = LEFT;
                    break;
                case BUTTON_BRIGHTNESS_UP:
                    brightnessUp();
                    break;
                case BUTTON_BRIGHTNESS_DOWN:
                    brightnessDown();
                    break;
            }
        }
    }

    // Color
    if (buffer[1] == 'C') {
        int red = buffer[2];
        int green = buffer[3];
        int blue = buffer[4];

        Color newColor(red, green, blue);

        currentColor = newColor;
    }

    ble.waitForOK();

    return d;
}

void turnOnPixels(Vector<int>& pixels) {
    for (Vector<int>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
        turnOnPixel(*it);
    }
    ledStrip.show();
}

void turnOnPixel(int pixelId) {
    ledStrip.setPixelColor(pixelId, currentColor.green, currentColor.red, currentColor.blue, currentColor.white);
}

void brightnessUp() {
    brightness += 10;
    if (brightness > 255) {
        brightness = 255;
    }
}
void brightnessDown() {
    brightness -= 10;
    if (brightness < 10) {
        brightness = 10;
    }
}

void error(const __FlashStringHelper* err) {
    Serial.println(err);
    while (1)
        ;
}
