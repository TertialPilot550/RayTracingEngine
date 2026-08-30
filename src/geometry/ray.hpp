#pragma once

#include "vector.hpp"

class Ray {
    public:
        Ray() {}
        Ray(const point& origin, const vec3& direction, double time = 0) : orig(origin), dir(direction), tm(time) {}

        const point& origin() const  { return orig; }
        const vec3& direction() const { return dir; }

        double time() const { return tm; }

        point at(double t) const {
            return orig + t * dir;
        }

    private:
        point orig;
        vec3 dir;
        double tm;
};