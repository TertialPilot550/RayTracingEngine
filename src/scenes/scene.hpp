#pragma once
#include "../main.hpp"

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
    CollisionList objects;
    color background;

    // Default to a kind of sky blue
    Scene() : controls(CamControls()), background(176, 252, 255) {}
    
};



