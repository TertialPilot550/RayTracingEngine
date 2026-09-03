#pragma once

#include "../../../main.hpp"

class SolidColor : public Texture {
    public:

    SolidColor(const color& albedo) : albedo(albedo) {}

    SolidColor(double r, double g, double b) {
        color solid;
        solid[RED] = r;
        solid[GREEN] = g;
        solid[BLUE] = b;
        albedo = solid;
    }

    color value(double u, double v, const point& p) const override {
        return albedo;
    }

    private:

    color albedo;

};
