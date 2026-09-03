#pragma once

/**
 * External Libraries
 */

// PNG Wrapper Library
#include <png++/png.hpp>
#include "../lib/metal-cpp/Metal/Metal.hpp"


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

// Objects
#include "objects/collidable.hpp"
#include "objects/primitives/sphere.hpp"
#include "objects/primitives/triangle.hpp"
#include "objects/primitives/quad.hpp"
#include "objects/instance.hpp"
// Textures
#include "objects/surfaces/texture.hpp"
#include "objects/surfaces/textures/checker.hpp"
#include "objects/surfaces/textures/image_texture.hpp"
#include "objects/surfaces/textures/noise.hpp"
#include "objects/surfaces/textures/solid_color.hpp"
// Materials
#include "objects/surfaces/material.hpp"
#include "objects/surfaces/materials/dielectric.hpp"
#include "objects/surfaces/materials/diffuse_light.hpp"
#include "objects/surfaces/materials/lambertian.hpp"
#include "objects/surfaces/materials/metal.hpp"
// Surface
#include "objects/surfaces/surface.hpp"
// Triangle Meshes
#include "objects/composites/tiny_obj_loader/obj_loader_wrapper.hpp"

// Camera Controls (Documented)
#include "camera/cam_controls.hpp"

// Scenes (Documented)
#include "scenes/scene.hpp"
#include "scenes/demos/triangle_demo.hpp"
#include "scenes/demos/sphere_demo.hpp"
#include "scenes/demos/light_demo.hpp"
#include "scenes/demos/model_demo.hpp"


// Camera (Documented)
#include "camera/camera_rig.hpp"

// Instancing and Motion
