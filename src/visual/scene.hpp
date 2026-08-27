#pragma once

#include "../lead.hpp"

class Scene {

    public:

    CollisionList objects;


    
};

class DemoScene : public Scene {

    public:

    DemoScene() {
       auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
        auto material_center = std::make_shared<Lambertian>(color(0.1, 0.2, 0.5));
        auto material_left   = std::make_shared<Dielectric>(1.50);
        auto material_bubble = std::make_shared<Dielectric>(1.00 / 1.50);
        auto material_right  = std::make_shared<Metal>(color(0.8, 0.6, 0.2), 0.0);

        objects.add(std::make_shared<Sphere>(point( 0.0, -100.5, -1.0), 100.0, material_ground));
        objects.add(std::make_shared<Sphere>(point( 0.0,    0.0, -1.2),   0.5, material_center));
        objects.add(std::make_shared<Sphere>(point(-1.0,    0.0, -1.0),   0.5, material_left));
        objects.add(std::make_shared<Sphere>(point(-1.0,    0.0, -1.0),   0.4, material_bubble));
        objects.add(std::make_shared<Sphere>(point( 1.0,    0.0, -1.0),   0.5, material_right));
    }

    

};