#pragma once

/**
 * External Libraries
 */

// PNG Wrapper Library
#include <png++/png.hpp>
// Metal C++ Library
#if defined(__APPLE__)
    #include "../lib/metal-cpp/Metal/Metal.hpp"
#endif

/**
 * Standard Library
 */

#include <memory>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <random>
#include <limits>

/**
 * Internal Headers
 */
// Utilites (Documented)
#include "utilities/util.hpp"
// All spatial math in this project uses the homogeneous convention defined here:
// - point = [1, x, y, z]
// - vector = [0, x, y, z]
#include "utilities/linear_algebra.hpp"
#include "utilities/interval.hpp"
#include "utilities/perlin_noise.hpp"
#include "utilities/ray.hpp"
#include "utilities/rgb.hpp"

// Acceration (Documented)
#include "acceleration/task_master.hpp"
#include "acceleration/gpu.hpp"

// Collision records must be visible to materials and surfaces.
#include "entities/objects/collidable.hpp"

// Textures
#include "entities/objects/surfaces/texture.hpp"
#include "entities/objects/surfaces/textures/checker.hpp"
#include "entities/objects/surfaces/textures/image_texture.hpp"
#include "entities/objects/surfaces/textures/noise.hpp"
#include "entities/objects/surfaces/textures/solid_color.hpp"
// Materials
#include "entities/objects/surfaces/material.hpp"
#include "entities/objects/surfaces/materials/dielectric.hpp"
#include "entities/objects/surfaces/materials/diffuse_light.hpp"
#include "entities/objects/surfaces/materials/lambertian.hpp"
#include "entities/objects/surfaces/materials/metal.hpp"
// Surface
#include "entities/objects/surfaces/surface.hpp"

// Objects
#include "entities/objects/instance.hpp"
#include "entities/objects/motion.hpp"

#include "entities/objects/primitives/geometric_primitive.hpp"
#include "entities/objects/primitives/sphere.hpp"
#include "entities/objects/primitives/triangle.hpp"
#include "entities/objects/primitives/quad.hpp"

// Triangle Meshes
#include "entities/objects/composites/tiny_obj_loader/obj_loader_wrapper.hpp"

// Camera Controls (Documented)
#include "camera/cam_controls.hpp"

// Scenes (Documented)
#include "entities/scenes/scene.hpp"
#include "entities/scenes/demos/triangle_demo.hpp"
#include "entities/scenes/demos/sphere_demo.hpp"
#include "entities/scenes/demos/light_demo.hpp"
#include "entities/scenes/demos/model_demo.hpp"


// Camera (Documented)
#include "camera/camera_rig.hpp"

// Instancing and Motion
