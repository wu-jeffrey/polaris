#include <Adafruit_NeoPixel.h>
#include <Vector.h>

#include <Arduino.h>
#include <SPI.h>
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

enum Directions { LEFT,
                  RIGHT,
                  FORWARD };


// Settings that can be changed via BlueTooth ////////////////////////////////////////////
Color currentColor = rgbColors[WHITE];

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
    ledStrip.setBrightness(brightness);

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

    if ( !ble.begin(VERBOSE_MODE) )
    {
        error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
    }
    Serial.println( F("OK!") );

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
    while (! ble.isConnected()) {
        delay(500);
    }
}

void loop() {
    switch (pollInstruction()) {
        // TODO: Decide what to do with no data
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
    delay(10);
}

int pollInstruction() {
    // Check for incoming characters from Bluefruit
    ble.println("AT+BLEUARTRX");
    ble.readline();
    if (strcmp(ble.buffer, "OK") == 0) {
        // no data
        return;
    }
    // Some data was found, its in the buffer
    enum Directions d;
    switch (ble.buffer) {
        case "0001": d = FORWARD; break;
        case "0002": d = RIGHT; break;
        case "0003": d = LEFT; break;
    }
    ble.waitForOK();

    return FORWARD;
}

void turnOnPixels(Vector<int>& pixels) {
    ledStrip.clear();
    for (Vector<int>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
        turnOnPixel(*it);
    }
    ledStrip.show();
}

void turnOnPixel(int pixelId) {
    ledStrip.setPixelColor(pixelId, currentColor.green, currentColor.red, currentColor.blue, currentColor.white);
}
