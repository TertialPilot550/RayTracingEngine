#pragma once
#include "../../lead.hpp"

using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

class ImageTexture : public Texture {

    public:

    ImageTexture(const char* filename) : img(filename) {}

    color value(double u, double v, const point& p) const override {
        if (img.get_height() <= 0) return color(0,1,1);

        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);

        auto i = int(u * img.get_width());
        auto j = int(v * img.get_height());
        auto pixel = img[i][j];

        auto color_scale = 1.0 / 255.0;

        return color(color_scale*pixel.red, color_scale*pixel.green, color_scale*pixel.blue);

    }

    private:

    Image img;

};