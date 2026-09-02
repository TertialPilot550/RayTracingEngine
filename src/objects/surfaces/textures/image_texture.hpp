#pragma once
#include "../../../main.hpp"

using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

class ImageTexture : public Texture {

    public:

    ImageTexture(const char* filename) : img(filename) {}

    color value(double u, double v, const point& p) const override {
        if (img.get_height() <= 0) {
            color res;
            res[RED] = 0;
            res[GREEN] = 1;
            res[BLUE] = 1;
            return res;
        }
        
        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);

        auto i = int(u * img.get_width());
        auto j = int(v * img.get_height());
        auto pixel = img[i][j];

        auto color_scale = 1.0 / 255.0;

        color res;
        res[RED] = color_scale*pixel.red;
        res[GREEN] = color_scale*pixel.green;
        res[BLUE] = color_scale*pixel.blue;

        return res;

    }

    private:

    Image img;

};