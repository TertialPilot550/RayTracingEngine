#pragma once
#include "../../../main.hpp"

/**
 * @brief Abstract Material class that describes how light bounces off of / interacts with the object
 */
class Material {
  public:
    virtual ~Material() = default;

    virtual color emitted(double, double, const point&) const {
        return color();
    }

    virtual bool scatter(const Ray&, const CollisionRecord&, color&, Ray&) const {
        return false;
    }

    virtual color at(int, int, const point&) const {
        return color();
    }
};