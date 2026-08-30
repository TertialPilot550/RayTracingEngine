#pragma once

#include "../../lead.hpp"

class SolidColor : public Texture {
    public:

    SolidColor(const color& albedo) : albedo(albedo) {}

    SolidColor(double r, double g, double b) : SolidColor(color(r, g, b)) {}

    color value(double u, double v, const point& p) const override {
        return albedo;
    }

    private:

    color albedo;

};
