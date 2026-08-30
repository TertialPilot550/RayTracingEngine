#pragma once
#include "../../lead.hpp"

class NoiseTexture : public Texture {
    public:
    NoiseTexture(double scale) : scale(scale) {}

    color value(double u, double v, const point& p) const override {
        return color(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

  private:
    Perlin noise;
    double scale;
};
