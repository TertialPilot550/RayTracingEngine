#pragma once
#include "../../main.hpp"

inline Scene model_demo() {
    Scene s;

    s.controls.set_image_width(1200);
    s.controls.set_samples_per_pixel(10);
    s.controls.set_image_width(1200);
    s.controls.set_vertical_fov(20);
    s.controls.set_camera_pos(point(10,-50, 10), point(1,0,0,0), point(1,0,0,1));
    s.controls.set_defocus_angle(0.6);
    s.controls.set_focus_distance(50.0);
    s.controls.max_depth = 50;
    s.background = color(0,0,0);

    auto difflight = std::make_shared<DiffuseLight>(color(4,4,4));
    auto ground_material = std::make_shared<Lambertian>(color(0.1, 0.5, 0.1));
    s.objects.add(std::make_shared<Sphere>(point(1,0,0,8), 2, difflight));
    s.objects.add(std::make_shared<Sphere>(point(1,0,0,-1000), 1000, ground_material));

    OBJModel model;
    load_model(model, "res/cube.obj");
    CollisionList l = model.convert_to_native_object();
    s.objects.add(std::make_shared<CollisionList>(l));

    return s;
}