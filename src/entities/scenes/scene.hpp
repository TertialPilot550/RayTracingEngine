#pragma once
#include "../../main.hpp"

/**
 * @brief Object which represents all of the context for a particular instance
 * of the ray tracing algorithm, including objects, background, and camera
 * details.
 * 
 * @details Contains a list of objects, a background, and camera controls.
 * Background color defaults to sky blue
 * 
 * @see CamControls, CollisionList, color
 */
class Scene {

    public:
    CamControls controls;
    InstanceList objects;
<<<<<<< HEAD
<<<<<<< HEAD
    
    color background_color = color(0.5, 0.7, 1.0);
    Scene() : controls(CamControls()), background_color(color(0.5, 0.7, 1.0)) {}
=======
    color background = color(0.5, 0.7, 1.0);
    Scene() : controls(CamControls()), background(color(0.5, 0.7, 1.0)) {}
>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)
=======
    color background = color(0.5, 0.7, 1.0);
    Scene() : controls(CamControls()), background(color(0.5, 0.7, 1.0)) {}
>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)

    color background_at(double u, double v, const point& p) {
        return background;
    }
    
};


