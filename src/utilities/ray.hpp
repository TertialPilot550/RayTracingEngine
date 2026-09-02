#pragma once

#include "../main.hpp"

/**
 * @brief Represents a ray used in the ray tracing algorithm
 */
class Ray {
    public:
        Ray() {}
        Ray(const point& origin, const point& direction, double time = 0) : orig(origin), dir(direction), tm(time) {}

        const point& origin() const  { return orig; }
        const point& direction() const { return dir; }

        double time() const { return tm; }

        point at(double t) const {
            return orig + t * dir;
        }

    private:
        point orig;
        point dir;
        double tm;
};