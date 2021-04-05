#include <Arduino.h>

class Color {
   public:
    int red;
    int green;
    int blue;
    int white;

    Color(int r, int g, int b) {
        if (r == 255 && g == 255 && b == 255) {
            init(0, 0, 0, 255);
        } else {
            init(r, g, b, 0);
        }
    }

    Color(int r, int g, int b, int w) {
        init(r, g, b, w);
    }

    void init(int r, int g, int b, int w) {
        red = r;
        green = g;
        blue = b;
        white = w;
    }
};

enum Colors { RED = 0,
              GREEN = 1,
              BLUE = 2,
              WHITE = 3 };

const Color basicColors[4]{
    {255, 0, 0, 0}, {0, 255, 0, 0}, {0, 0, 255, 0}, {0, 0, 0, 255}};
