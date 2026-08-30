#pragma once

#include "../lead.hpp"

class Texture {
    public:

    virtual ~Texture() = default;
    virtual color value(double u, double v, const point& p) const = 0;
};