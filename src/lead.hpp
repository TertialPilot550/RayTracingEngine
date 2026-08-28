#pragma once

// PNG Wrapper Library
#include <png++/png.hpp>
// Standard Library
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <random>


// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double radians_to_degrees(double rad) {
    return (rad * 180.0)/pi;
}

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

inline double random_double() {

    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);

}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}


// User Headers
#include "geometry/interval.hpp"
#include "geometry/ray.hpp"
#include "geometry/collidable.hpp"
#include "geometry/vector.hpp"
#include "geometry/sphere.hpp"
#include "geometry/triangle.hpp"

#include "visual/material.hpp"
#include "visual/scene.hpp"
#include "visual/task_master.hpp"
#include "visual/camera_rig.hpp"
