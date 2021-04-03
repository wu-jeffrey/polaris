class Color {
   public:
    int red;
    int green;
    int blue;
    int white;

    Color(int r, int g, int b, int w) {
        red = r;
        green = g;
        blue = b;
        white = w;
    }
};

enum Colors { RED,
              GREEN,
              BLUE,
              WHITE };

const Color rgbColors[4]{
    {255, 0, 0, 0}, {0, 255, 0, 0}, {0, 0, 255, 0}, {0, 0, 0, 255}};