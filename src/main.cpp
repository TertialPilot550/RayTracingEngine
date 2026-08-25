#include "lead.hpp"


int main() {
    DemoScene d;
    Camera c;
    png::image<png::rgb_pixel> image = c.capture(d);
    image.write("output.png");
    return 0;
}