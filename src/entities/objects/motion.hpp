#pragma once

#include "../../main.hpp"

/**
 * A time-varying object transform.  The current renderer uses affine
 * transforms, so interpolation is performed component-wise and clamped to
 * the configured shutter interval.
 */
class Motion {
public:
    mat<4, 4> start;
    mat<4, 4> end;
    double start_time = 0.0;
    double stop_time = 1.0;

    Motion() {
        start = 0.0;
        end = 0.0;
        for (int i = 0; i < 4; ++i) {
            start[i][i] = 1.0;
            end[i][i] = 1.0;
        }
    }

    Motion(const mat<4, 4>& start_transform, const mat<4, 4>& end_transform,
           double start_time = 0.0, double stop_time = 1.0)
        : start(start_transform), end(end_transform),
          start_time(start_time), stop_time(stop_time) {}

    mat<4, 4> at(double time) const {
        if (stop_time <= start_time) return start;
        const double amount = std::clamp((time - start_time) /
                                         (stop_time - start_time), 0.0, 1.0);
        return (1.0 - amount) * start + amount * end;
    }

    point transform(const point& value, double time) const {
        return at(time) * value;
    }
};
