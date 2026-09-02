#pragma once
#include "../../../main.hpp"

class NoiseTexture : public Texture {
    public:
    NoiseTexture(double scale) : scale(scale) {}

    color value(double u, double v, const point& p) const override {      
        point hp = homogenize(p);
        
        color s;
        s[RED] = 0.5;
        s[GREEN] = 0.5;
        s[BLUE] = 0.5;

        return s * (1 + std::sin((scale * (hp[Z]) + (10 * noise.turb(hp, 7)))));
    }

  private:
    Perlin noise;
    double scale;
};
