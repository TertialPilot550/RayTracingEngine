#pragma once
#include "../../main.hpp"

class Material {
  public:
    virtual ~Material() = default;

    virtual color emitted(double u, double v, const point& p) const {
        return color(0,0,0);
    }

    virtual bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered) const {
        return false;
    }
};