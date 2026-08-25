#pragma once
#include <png++/png.hpp>

#include "lead.hpp"
#include "geometry/sphere.hpp"
class Scene {

    public:

    CollisionList objects;


    
};

class DemoScene : public Scene {

    public:

    DemoScene() {
        objects.add(make_shared<Sphere>(point(0,0,-1), 0.5));
        objects.add(make_shared<Sphere>(point(0,-100.5,-1), 100));
    }

    

};