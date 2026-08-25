#pragma once
#include "../lead.hpp"


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

const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);
