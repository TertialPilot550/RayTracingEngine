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
#include "tiny_obj_loader/tiny_obj_loader.h"
#include "tiny_obj_loader/obj_loader_wrapper.hpp"

// Constants

/**
 * @brief the value of infinity
 */
const double infinity = std::numeric_limits<double>::infinity();
/**
 * @brief the value of pi
 */
const double pi = 3.1415926535897932385;

/**
 * @brief Conversion from degrees to radians
 */
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

/**
 * @brief Conversion from radian to degress
 */
inline double radians_to_degrees(double rad) {
    return (rad * 180.0)/pi;
}

/**
 * @brief Color correction utility
 * @details The justification for this can be found in ray tracing on one weekend.
 */
inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

/**
 * @brief Random double in the range [0, 1)
 * @details thread_local random state to allow parralelization of tasks dependent on random numbers
 */
inline double random_double() {

    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);

}

/**
 * @brief Random double in an arbitrary range [min, max)
 */
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

/**
 * @brief Random int in an arbitrary range [min, max)
 */
int random_int(int min, int max) {
    // 1. Thread-local seed source and engine initialization (happens once per thread)
    thread_local std::random_device rd;
    thread_local std::mt19937 generator(rd());
    
    // 2. Define the distribution range
    std::uniform_int_distribution<int> distribution(min, max);
    
    // 3. Generate the number
    return distribution(generator);
}

// User Headers
#include "geometry/interval.hpp"
#include "geometry/ray.hpp"
#include "geometry/collidable.hpp"

// Geometric Primitives
#include "geometry/vector.hpp"
#include "geometry/sphere.hpp"
#include "geometry/triangle.hpp"
#include "geometry/quad.hpp"

#include "noise/perlin_noise.hpp"




// Texture Superclass
#include "surfaces/texture.hpp"
// Texture Types
#include "surfaces/textures/solid_color.hpp"
#include "surfaces/textures/checker.hpp"
#include "surfaces/textures/noise.hpp"
#include "surfaces/textures/image_texture.hpp"

// Scene
#include "scenes/scene.hpp"

// Material Superclass
#include "surfaces/material.hpp"
// Material Types
#include "surfaces/materials/lambertian.hpp"
#include "surfaces/materials/metal.hpp"
#include "surfaces/materials/dielectric.hpp"
#include "surfaces/materials/diffuse_light.hpp"


// Rendering 
#include "render/task_master.hpp"
#include "render/camera_rig.hpp"
