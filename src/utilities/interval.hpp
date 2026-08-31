#pragma once
#include "../main.hpp"

/**
 * @brief Represents an interval of real numbers, used through 
 * the ray tracing algorithm.
 */
class Interval {

    public:
    
    double max, min;

    Interval(): min(+infinity), max(-infinity) {}

    Interval(double min, double max) :min(min), max(max) {}
    
    double size() {
        return max - min;
    }

    bool contains(double x) {
        return min <= x && x <= max;
    }

    bool surrouds(double x) {
        return min <= x && x <= max;
    }

    double clamp(double x) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const Interval empty, universe;
};

inline const Interval Interval::empty = Interval(+infinity, -infinity);
inline const Interval Interval::universe = Interval(-infinity, +infinity);
