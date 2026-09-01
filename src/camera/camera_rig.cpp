#include "camera_rig.hpp"


Image& CameraRig::capture(Scene& s) {
    if (img_buffer) delete img_buffer;
    img_buffer = new Image(s.controls.img_w(), s.controls.img_h());
    
    render(s);

    return (*img_buffer);
}

void CameraRig::render(Scene& s) {
    TaskMaster tm(s.controls.thread_count);
    std::vector<std::function<void()>> tasks;

    // For each chunk...
    for (int y = 0; y < s.controls.img_h(); y++) {
        for (int x = 0; x < s.controls.img_w(); x+= s.controls.chunk_size) {
            // Create a function object encapsulating the computation for that chunk
            tasks.emplace_back([this, x, y, &s]() {
                // Calculate chunk values locally before write back to avoid false cache sharing
                rgb res[s.controls.chunk_size];

                // Calculate pixel values for each pixel in the chunk
                for (int i = 0; i < s.controls.chunk_size && x+i < s.controls.img_w(); i++) {
                    res[i] = sample_for_pixel_color(x+i, y, s);
                }

                // Write to buffer after to take advanatage of locality
                // !! Impure !!
                for (int i = 0; i < s.controls.chunk_size && x+i < s.controls.img_w(); i++) {
                    write_color(res[i], x+i, y);
                }

            });
        }
    }

    // Perform rendering using threads
    tm.dispatch(tasks);
}

void CameraRig::write_color(const rgb& rgb, int x, int y) {
    if (img_buffer) (*img_buffer)[y][x] = png::rgb_pixel(rgb.r, rgb.g, rgb.b);
}

color CameraRig::process_ray(const Ray& r, int depth, Scene& s) {
    CollisionRecord rec;
    if (depth <= 0) {
        return color(0, 0, 0);
    }

    static bool only_lights = false;

    if (only_lights) {
        // If the ray hits nothing, return the background color.
        if (!s.objects.hit(r, Interval(0.001, infinity), rec))
            return s.background;

        Ray scattered;
        color attenuation;

        if (!rec.mat) {
            auto b = std::make_shared<SolidColor>(0,0,0);
            auto p = std::make_shared<SolidColor>(235, 116, 237);

            auto check = std::make_shared<CheckerTexture>(2.0, b, p);
            std::shared_ptr<Material> m = std::make_shared<Lambertian>(check);
            rec.mat = m;
        }

        
        color color_from_emission = rec.mat->emitted(rec.t_coords.f1, rec.t_coords.f2, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * process_ray(scattered, depth-1, s);

        return color_from_emission + color_from_scatter;
    } else {
        if (s.objects.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            color attenuation;
            if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * process_ray(scattered, depth-1, s);
            return color(0,0,0);
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        // manually blend the background color with white to create a gradient effect
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
    
}

vec3 CameraRig::sample_square() const {
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

point CameraRig::defocus_disk_sample(CamControls& controls) const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return controls.center() + (p[0] * controls.defocus_radius_u()) + (p[1] * controls.defocus_radius_v());
}

Ray CameraRig::get_ray_for_pixel(int i, int j, CamControls& controls) const {
    auto offset = (controls.do_antialiasing) ? sample_square() : vec3(0,0,0);
    auto pixel_sample = controls.viewport_origin() + ((i + offset.x()) * controls.du()) + ((j + offset.y()) * controls.dv());
    auto ray_origin = (controls.defcs_angle() <= 0) ? controls.center() : defocus_disk_sample(controls);
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();
    return Ray(ray_origin, ray_direction, ray_time);
}

rgb CameraRig::sample_for_pixel_color(int x, int y, Scene& s) {
    color pixel_color(0, 0, 0);
    for (int i = 0; i < s.controls.samples_per_pix(); i++) {
        pixel_color += process_ray(get_ray_for_pixel(x, y, s.controls), s.controls.max_depth, s);
    }
    return color_correction_to_rgb(s.controls.pscale() * pixel_color);
}