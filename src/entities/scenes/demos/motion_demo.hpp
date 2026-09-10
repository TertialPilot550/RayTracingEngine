#pragma once

#include "../../../main.hpp"

inline mat<4, 4> translation_matrix(double x, double y, double z) {
    mat<4, 4> result;
    result[0][0] = 1.0;
    result[1][1] = 1.0;
    result[2][2] = 1.0;
    result[3][3] = 1.0;
    result[1][0] = x;
    result[2][0] = y;
    result[3][0] = z;
    return result;
}

inline Scene motion_demo() {
    Scene s;
    s.controls.mode = CameraMode::RAY_TRACING;
    s.controls.accel_mode = AccelerationMode::PARALLEL;
    s.controls.set_aspect_ratio(16.0 / 9.0);
    s.controls.set_image_width(640);
    s.controls.set_samples_per_pixel(40);
    s.controls.set_vertical_fov(35);
    s.controls.set_camera_pos(make_point(10, -18, 8), make_point(0, 0, 1), make_vector(0, 0, 1));
    s.controls.set_defocus_angle(0);
    s.controls.set_focus_distance(20);
    s.controls.max_depth = 20;
    s.background = color(0.02, 0.03, 0.07);

    auto ground = std::make_shared<Lambertian>(color(0.12, 0.16, 0.22));
    auto stationary_material = std::make_shared<Lambertian>(color(0.1, 0.7, 0.95));
    auto moving_material = std::make_shared<Lambertian>(color(0.95, 0.02, 0.01));

    s.objects.add(std::make_shared<Sphere>(
        make_point(0, 0, -1001), 1000, make_surface(ground)));

    s.objects.add(std::make_shared<Sphere>(
        make_point(-3, 0, 1.5), 1.5, make_surface(stationary_material)));

    auto moving_sphere = std::make_shared<Sphere>(
        make_point(0, 0, 1.5), 1.5, make_surface(moving_material));
    moving_sphere->set_motion(Motion(
        translation_matrix(0, 0, 0),
        translation_matrix(8, 0, 0)));
    s.objects.add(moving_sphere);

    auto light = std::make_shared<DiffuseLight>(color(4, 4, 4));
    s.objects.add(std::make_shared<Sphere>(
        make_point(0, 2, 8), 2, make_surface(light)));

    return s;
}
