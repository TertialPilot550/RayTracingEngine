#pragma once

#include "../main.hpp"


/**
 * @brief Simple RGB color object.
 * @details 3 integer fields and an = assignment operator
 */
class rgb {
    public:
    
    int r;
    int g;
    int b;

    rgb(): r(0), g(0), b(0) {}

    rgb(int r, int g, int b): r(r), g(g), b(b) {}

    void operator=(rgb color) {
        r = color.r;
        g = color.g;
        b = color.b;
    }
};

/**
 * @brief Conversion from color to rgb color object
 */
inline rgb color_correction_to_rgb(const color& pixel_color) {
        auto r = pixel_color[RED];
        auto g = pixel_color[GREEN];
        auto b = pixel_color[BLUE];

        // std::cout << r << g << b;

        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // std::cout << r << g << b;

        // Translate the [0,1] component values to the byte range [0,255].
        static Interval unit(0, 0.999);
        int rbyte = int(256 * unit.clamp(r));
        int gbyte = int(256 * unit.clamp(g));
        int bbyte = int(256 * unit.clamp(b));

        return rgb(rbyte, gbyte, bbyte);
}