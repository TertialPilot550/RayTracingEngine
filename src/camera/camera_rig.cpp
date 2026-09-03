#include "camera_rig.hpp"

/*
 * Rendering Behavior 
 */

// Pure
color process_ray_with_lighting(const Ray& r, int depth, Scene& s) {
    CollisionRecord rec;
    if (depth <= 0) return color();

    // If the ray hits nothing, return the background color.
    if (!s.objects.hit(r, Interval(0.001, infinity), rec))
        return s.background;

    Ray scattered;
    color attenuation;

    if (!rec.mat) {
        auto b = std::make_shared<SolidColor>(0,0,0);
        auto p = std::make_shared<SolidColor>(235, 116, 237);

        std::shared_ptr<Material> m = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(2.0, b, p));
        rec.mat = m;
    }

    color color_from_emission = rec.mat->emitted(rec.t_coords[0], rec.t_coords[1], rec.p);

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
        return color_from_emission;

    color color_from_scatter = attenuation * process_ray_with_lighting(scattered, depth-1, s);
    return color_from_emission + color_from_scatter;
}

// Pure
color process_ray_without_lighting(const Ray& r, int depth, Scene& s) {
    CollisionRecord rec;

    if (s.objects.hit(r, Interval(0.001, infinity), rec)) {
        color emitted = rec.mat ? rec.mat->emitted(rec.t_coords[0], rec.t_coords[1], rec.p) : color(0,0,0);
        Ray scattered;
        color attenuation;
        if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * process_ray_without_lighting(scattered, depth-1, s);
        return emitted;
    }
    vec<4> unit_direction = unit_vector(as_vector(r.direction()));
    auto a = 0.5*(unit_direction[Y] + 1.0);
    // manually blend the background color with white to create a gradient effect
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}


// Pure (can be split into chunks)
void rasterize_scene(Scene& s, double* depth_buffer, rgb* color_buffer) {
    int s_size[2];
    s_size[0] = s.controls.img_w();
    s_size[1] = s.controls.img_h();

    // Calculate transformation matricies
    Matrix<4,4> view = viewport_matrix(s_size[0], s_size[1]);
    Matrix<4,4> projection = projection_matrix(2, 10, 5, -5, -8, 8);
    Matrix<4,4> world_to_cam = world_to_camera_matrix(s.controls.u(), s.controls.v(), s.controls.w(), s.controls.center());
    Matrix<4,4> proj_to_cam = projection * world_to_cam;

    for (int i = 0; i < s.objects.objects.size(); i++) {
        s.objects.objects[i]->rasterize(s_size, view, proj_to_cam, depth_buffer, color_buffer);
    }
}

// Impure
// TODO: Incorporate chunking
void CameraRig::depth_buffer(Scene& s) {
    // Setup
    int w = s.controls.img_w();
    int h = s.controls.img_h();

    // Allocate and initialize the buffers
    double depth_buffer[w][h];
    rgb color_buffer[w][h];
    
    // Intialize the buffers to infinity/black
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            depth_buffer[i][j] = INFINITY;
            color_buffer[i][j] = rgb(0,0,0);
        }
    }

    // Depth Buffer and Rasterization Algorithms
    rasterize_scene(s, &depth_buffer[0][0], &color_buffer[0][0]);

    // Complete! Write to image.
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            write_color(color_buffer[i][j], i, j);
        }
    }

    // Drop the buffers
    delete depth_buffer;
    delete color_buffer;

}

/*
 * Member Functions 
 */

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

            // Change the rendering method depending on the camera mode
            switch(s.controls.mode) {
                case CameraMode::RAY_TRACING :
                    // Ray Tracing

                    // Create a function object encapsulating the computation for that chunk
                    tasks.emplace_back([this, x, y, &s]() {
                        // Calculate chunk values locally before write back to avoid false cache sharing
                        rgb res[s.controls.chunk_size];

                        // Calculate pixel values for each pixel in the chunk
                        for (int i = 0; i < s.controls.chunk_size && x+i < s.controls.img_w(); i++) {
                            res[i] = sample_for_pixel_color(x+i, y, s);
                        }

                        // Write to buffer after to take advanatage of locality
                        // !! Impure !! - BUT: does not effect any SHARED state
                        for (int i = 0; i < s.controls.chunk_size && x+i < s.controls.img_w(); i++) {
                            write_color(res[i], x+i, y);
                        }

                    });
                    break;

                case CameraMode::HYBRID :
                    // TODO
                    break;

                case CameraMode::DEPTH_BUFFER :
                    depth_buffer(s);
                    break;


            }
        }
    }

    // Perform rendering using threads
    tm.dispatch(tasks);
}

void CameraRig::write_color(const rgb& rgb, int x, int y) {
    if (img_buffer) (*img_buffer)[y][x] = png::rgb_pixel(rgb.r, rgb.g, rgb.b);
}

color CameraRig::process_ray(const Ray& r, int depth, Scene& s) {
    if (s.controls.do_lighting) {
        return process_ray_with_lighting(r, depth, s);
    } else {
        return process_ray_without_lighting(r, depth, s);
    }
}

point CameraRig::sample_square() const {
    vec<4> res;

    res[P] = 0;
    res[X] = random_double() - 0.5;
    res[Y] = random_double() - 0.5;
    res[Z] = 0;

    return res;
}

point CameraRig::defocus_disk_sample(CamControls& controls) const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk<4>();
    return controls.center() + (p[X] * controls.defocus_radius_u()) + (p[Y] * controls.defocus_radius_v());
}

Ray CameraRig::get_ray_for_pixel(int i, int j, CamControls& controls) const {
    vec<4> os;
    auto offset = (controls.do_antialiasing) ? sample_square() : os;
    auto pixel_sample = controls.viewport_origin() + ((i + offset[X]) * controls.du()) + ((j + offset[Y]) * controls.dv());
    auto ray_origin = (controls.defcs_angle() <= 0) ? controls.center() : defocus_disk_sample(controls);
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();
    return Ray(ray_origin, ray_direction, ray_time);
}

rgb CameraRig::sample_for_pixel_color(int x, int y, Scene& s) {
    color pixel_color = color();
    for (int i = 0; i < s.controls.samples_per_pix(); i++) {
        pixel_color += process_ray(get_ray_for_pixel(x, y, s.controls), s.controls.max_depth, s);
    }
    return color_correction_to_rgb(s.controls.pscale() * pixel_color);
}