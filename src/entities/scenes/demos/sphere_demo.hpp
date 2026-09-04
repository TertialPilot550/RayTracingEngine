#pragma once
#include "../../../main.hpp"

inline Scene sphere_demo() {
    Scene s;

    s.controls.set_image_width(400);
    s.controls.set_samples_per_pixel(5);
    s.controls.set_vertical_fov(20);
    s.controls.set_camera_pos(make_point(13,2,3), make_point(0,0,0), make_vector(0,1,0));
    s.controls.set_defocus_angle(0.6);
    s.controls.set_focus_distance(10.0);
    s.controls.max_depth = 50;

    auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
    s.objects.add(std::make_shared<Sphere>(make_point(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point center = make_point(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if (norm(center - make_point(4, 0.2, 0)) > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    s.objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    s.objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    s.objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    s.objects.add(std::make_shared<Sphere>(make_point(1, 0, 1), 1.0, material1));
    return s;
}