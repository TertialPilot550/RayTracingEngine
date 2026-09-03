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
class Scene : public CollisionObject {

    public:
    CamControls controls;
    std::vector<Instance> obj;
    std::shared_ptr<Texture> background;

    // Default to a black background
    Scene(std::shared_ptr<Texture> tex) : controls(CamControls()), background(tex) {}
    
};



