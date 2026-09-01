#pragma once
#include "../../main.hpp"
inline Scene triangle_demo() {
    Scene s;

    s.controls.set_image_width(1200);
    s.controls.set_samples_per_pixel(10);
    s.controls.set_image_width(1200);
    s.controls.set_vertical_fov(20);
    s.controls.set_camera_pos(point(10,-50, 10), point(0,0,0), vec3(0,0,1));
    s.controls.set_defocus_angle(0.6);
    s.controls.set_focus_distance(50.0);
    s.controls.max_depth = 50;
    s.background = color(0,0,0);

    auto difflight = std::make_shared<DiffuseLight>(color(4,4,4));
    auto ground_material = std::make_shared<Lambertian>(color(0.1, 0.5, 0.1));
    s.objects.add(std::make_shared<Sphere>(point(0,0,3), 7, difflight));
    s.objects.add(std::make_shared<Sphere>(point(0,0,-1000), 1000, ground_material));
    auto pertext = std::make_shared<NoiseTexture>(4);
    auto l = std::make_shared<Lambertian>(pertext);

    for (int i = 0; i < 10; i++) {
        s.objects.add(std::make_shared<Triangle>(point((i-5)*5, (i-5)*5, 0), point(((i-5)*5)+2, ((i-5)*5)+2, 0), point(((i-5)*5)+1, ((i-5)*5)+1, 5), l));
    }


    return s;
}

