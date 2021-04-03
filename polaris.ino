#include <Adafruit_NeoPixel.h>
#include <Vector.h>

#include "color.cpp"

#define PIN 6
#define N_LEDS 24
#define N_LEDS_PER_STICK 8

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_RGBW + NEO_KHZ800);

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
    Serial.begin(9600);
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
}

void loop() {
    switch (pollInstruction()) {
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
