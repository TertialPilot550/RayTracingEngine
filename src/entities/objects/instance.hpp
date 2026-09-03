#pragma once

#include "../main.hpp"

class Instance : public CollisionObject {

    public:

    std::shared_ptr<CollisionObject> obj;
    std::shared_ptr<Surface> obj;
    Matrix<4,4> transform;

};

class Motion {

    public:
    Matrix<4,4> start, end;
    double start_time, stop_time;

    // rough, linear interpolation; might want to redo this
    Matrix<4,4> at(double t) {
        double len = stop_time-start_time;
        if (len == 0) len = 1;
        double p = t / (stop_time-start_time);
        Interval(0, 1).clamp(p);

        return (1-p)*start + end*p;
    }

    vec<4> transform(const vec<4> v, double t) {
        return at(t) * v;
    }
};
