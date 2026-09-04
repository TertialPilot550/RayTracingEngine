#pragma once

#include "../../../main.hpp"

/**
 * @brief Abstract texture class meant to describe the appearance of an object
 */
class Texture {
    public:

    virtual ~Texture() = default;
    virtual color value(double u, double v, const point& p) const = 0;
};