#pragma once
#include "../../../main.hpp"

inline Scene light_demo() {
    Scene s;
    auto pertext = std::make_shared<NoiseTexture>(4);
    s.objects.add(std::make_shared<Sphere>(make_point(0,-1000,0), 1000, std::make_shared<Lambertian>(pertext)));
    s.objects.add(std::make_shared<Sphere>(make_point(0,2,0), 2, std::make_shared<Lambertian>(pertext)));

    auto difflight = std::make_shared<DiffuseLight>(color(4,4,4));
    s.objects.add(std::make_shared<Sphere>(make_point(0,7,0), 2, difflight));
    s.objects.add(std::make_shared<Quad>(make_point(3,1,-2), make_vector(2,0,0), make_vector(0,2,0), difflight));

    s.controls.set_aspect_ratio(16.0/9.0);
    s.controls.set_image_width(400);
    s.controls.set_samples_per_pixel(10);
    s.controls.set_vertical_fov(20);
    s.controls.set_camera_pos(make_point(26,3,6), make_point(0,2,0), make_vector(0,1,0));
    s.controls.set_defocus_angle(0);
    s.controls.max_depth = 50;
    s.background = color(0,0,0);
    
    return s;
}