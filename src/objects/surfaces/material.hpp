#pragma once
#include "../../main.hpp"

/**
 * @brief Abstract Material class that describes how light bounces off of / interacts with the object
 */
class Material {
  public:
    virtual ~Material() = default;

    virtual color emitted(double u, double v, const point& p) const {
        return color();
    }

    virtual bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered) const {
        return false;
    }
};