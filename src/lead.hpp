#pragma once

// Standard Library
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>

// User Headers
#include "geometry/ray.hpp"
#include "geometry/collidable.hpp"
#include "geometry/vector.hpp"

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


