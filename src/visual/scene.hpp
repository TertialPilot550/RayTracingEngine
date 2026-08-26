#pragma once

#include "../lead.hpp"

class Scene {

    public:

    CollisionList objects;


    
};

class DemoScene : public Scene {

    public:

    DemoScene() {
        auto ground_material = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
        objects.add(make_shared<Sphere>(point(0,-1000,0), 1000, ground_material));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = random_double();
                point center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

                if ((center - point(4, 0.2, 0)).length() > 0.9) {
                    shared_ptr<Material> sphere_material;

                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = color::random() * color::random();
                        sphere_material = make_shared<Lambertian>(albedo);
                        objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    } else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = make_shared<Metal>(albedo, fuzz);
                        objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    } else {
                        // glass
                        sphere_material = make_shared<Dielectric>(1.5);
                        objects.add(make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = make_shared<Dielectric>(1.5);
        objects.add(make_shared<Sphere>(point(0, 1, 0), 1.0, material1));

        auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
        objects.add(make_shared<Sphere>(point(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
        objects.add(make_shared<Sphere>(point(4, 1, 0), 1.0, material3));
    }

    

};