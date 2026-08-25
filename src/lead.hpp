#pragma once

// PNG Wrapper Library
#include <png++/png.hpp>
// Standard Library
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>


// C++ Std Usings
using std::make_shared;
using std::shared_ptr;

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

inline double random_double() {
    return std::rand() / (RAND_MAX + 1.0);
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
#include "scene.hpp"
#include "camera.hpp"