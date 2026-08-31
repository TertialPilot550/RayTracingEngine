#pragma once

#include "../../../main.hpp"

class CheckerTexture : public Texture {

    public:

    CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) : inv_scale(1.0 /scale), even(even), odd(odd) {}


    color value(double u, double v, const point& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }
    
    private:

    double inv_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};