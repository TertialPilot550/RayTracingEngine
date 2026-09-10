#include "camera_rig.hpp"



// SHORT TODO:
// - Motion / Transformations on objects

// Theres an issue with motions now???? how am i gonna do this. motions aren't integrated with any rendering type, and need to be implemented in a way
// that the objects internal thing? idk

// objects need a link to a motion? somehow?
// and motions need a link to objects?

// should motions be part of the scene or the object?
// when I have skeltons, it'll be part of the object and the scene???????????
// an internal motion linking the parts, which is not time based, and time based transformations on each individual part/the whole things
// so it should be external
// plus then multiple objects can use the same motion

/*
Depth Buffer: account for shutter events
    - For each shutter event start point:
        - generate a picture, somehow combining results across the interval
            - needs more consideration
        - move onto the next picture
*/

// ---------------------------- High Level Interface ---------------------------- //


// Capture a given scene
Image& CameraRig::capture(Scene& s) {
    if (img_buffer) delete img_buffer;
    img_buffer = new Image(s.controls.img_w(), s.controls.img_h());

    // For each shutter event, render that event
    if (s.controls.get_shutter_events()) {
        for (int i = 0; i < s.controls.get_shutter_event_count(); i++) {
            Interval shutter_event = s.controls.get_shutter_events()[i];
            render(s, shutter_event);
        }
    }

    return (*img_buffer);
}

void CameraRig::render_locally(Scene& s, Interval shutter_event) {
    // For each pixel
    switch (s.controls.mode) {
        case CameraMode::RAY_TRACING :
            ray_trace(s, shutter_event);    
            break;
        case CameraMode::HYBRID :

            break;
        case CameraMode::DEPTH_BUFFER :
            depth_buffer(s, shutter_event);
            break;
    }
}

void CameraRig::render_parallel(Scene& s, Interval shutter_event) {
    TaskMaster tm(s.controls.thread_count);
    std::vector<std::function<void()>> tasks;

    // For each chunk, render that chunk
    for (int y = 0; y < s.controls.img_h(); y++) {
        for (int x = 0; x < s.controls.img_w(); x+= s.controls.chunk_size) {

            // Change the rendering method depending on the camera mode
            switch(s.controls.mode) {
                case CameraMode::RAY_TRACING :
                    // Ray Tracing Rendering
                    tasks.emplace_back(std::bind(ray_trace_worker, s, x, y, shutter_event));
                    break;

                case CameraMode::HYBRID :
                    // Hybrid Rendering
                    tasks.emplace_back(std::bind(hybrid_worker, s, x, y, shutter_event));
                    break;

                case CameraMode::DEPTH_BUFFER :
                    // Depth Buffer Rendering
                    tasks.emplace_back(std::bind(depth_buffer_worker, s, x, y, shutter_event));
                    break;
            }

        }
    }

    // Perform rendering using threads
    tm.dispatch(tasks);
}

void render_gpu(Scene& s, Interval shutter_event) {
    throw new std::runtime_error("GPU Acceleration not yet implemented.");
}


// Render a given scene depending on the acceleration type specified in CamControls
void CameraRig::render(Scene& s, Interval shutter_event) {
    switch(s.controls.accel_mode) {
        case AccelerationMode::NONE:
            render_locally(s, shutter_event);
            break;
        case AccelerationMode::PARALLEL:
            render_parallel(s, shutter_event);
            break;
        case AccelerationMode::GPU:
            render_gpu(s, shutter_event);
            break;
    }
}

// ---------------------------- Ray Tracing ---------------------------- //

// Ray tracing without any acceleration
void CameraRig::ray_trace(Scene& s, Interval shutter_event) {
    for (int y = 0; y < s.controls.img_h(); y++) {
        for (int x = 0; x < s.controls.img_w(); x++) {
            write_color(sample_for_pixel_color(x, y, shutter_event, s), x, y);
        }
    } 
}

// Ray tracing worker function for parralelization
void CameraRig::ray_trace_worker(Scene& s, int s_x, int s_y, Interval shutter_event) {
    // Build Buffer
    int chunk_size = s.controls.chunk_size;
    rgb res[chunk_size];

    // Calculate into Buffer
    for (int i = 0; i < chunk_size && s_x + i < s.controls.img_w(); i++) {
        res[i] = sample_for_pixel_color(s_x+i, s_y, shutter_event, s);
    }

    // Write Back From Buffer
    for (int i = 0; i < chunk_size && s_x + i < s.controls.img_w(); i++) {
        write_color(res[i], s_x+i, s_y);
    }
}

/*
 * Helpers
 */

// (Pure) Get a ray to send to pixel (i, j)
Ray CameraRig::get_ray_for_pixel(int i, int j, Interval shutter_event, CamControls& controls) const {
    // Determine the ray parameters
    point offset = (controls.do_antialiasing) ? anti_aliasing_offset() : make_vector(0,0,0);

    // Construct the ray
    point r_origin = (controls.defcs_angle() <= 0) ? controls.center() : random_point_on_camera_lens(controls);
    point r_towards = controls.viewport_origin() + ((i + offset[X]) * controls.du()) + ((j + offset[Y]) * controls.dv());
    point r_direction = r_towards - r_origin;
    double r_time = random_double(shutter_event.min, shutter_event.max);
    return Ray(r_origin, r_direction, r_time);
}

// Pure
rgb CameraRig::sample_for_pixel_color(int x, int y, Interval shutter_event, Scene& s) {
    color pixel_color = color();
    for (int i = 0; i < s.controls.samples_per_pix(); i++) {
        pixel_color += process_ray(get_ray_for_pixel(x, y, shutter_event, s.controls), s.controls.max_depth, s);
    }
    return color_correction_to_rgb(s.controls.pscale() * pixel_color);
}

// Pure
color CameraRig::process_ray(const Ray& r, int depth, Scene& s) {
    if (s.controls.do_lighting) {
        return process_ray_with_lighting(r, depth, s);
    } else {
        return process_ray_without_lighting(r, depth, s);
    }
}

// (Pure) Process ray using only declared light sources
color process_ray_with_lighting(const Ray& r, int depth, Scene& s) {
    CollisionRecord rec;
    if (depth <= 0) return color();

    // If the ray hits nothing, return the background color.
    if (!s.objects.hit(r, Interval(0.001, infinity), rec))
        return s.background;

    Ray scattered;
    color attenuation;

    // Default texture assingment for ray tracing
    if (!rec.surf) {
        auto b = std::make_shared<SolidColor>(0,0,0);
        auto p = std::make_shared<SolidColor>(235, 116, 237);

        std::shared_ptr<Material> m = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(2.0, b, p));
        rec.surf = std::make_shared<Surface>(m, std::make_shared<CheckerTexture>(2.0, b, p));
    }

    color color_from_emission = rec.surf->emitted(rec.t_coords[0], rec.t_coords[1], rec.p);

    if (!rec.surf->scatter(r, rec, attenuation, scattered))
        return color_from_emission;

    color color_from_scatter = attenuation * process_ray_with_lighting(scattered, depth-1, s);
    return color_from_emission + color_from_scatter;
}

// (Pure) Process ray using ambient lighting
color process_ray_without_lighting(const Ray& r, int depth, Scene& s) {
    CollisionRecord rec;

    if (s.objects.hit(r, Interval(0.001, infinity), rec)) {
        color emitted = rec.surf ? rec.surf->emitted(rec.t_coords[0], rec.t_coords[1], rec.p) : color(0,0,0);
        Ray scattered;
        color attenuation;
        if (rec.surf && rec.surf->scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * process_ray_without_lighting(scattered, depth-1, s);
        return emitted;
    }
    vec<4> unit_direction = unit_vector(as_vector(r.direction()));
    auto a = 0.5*(unit_direction[Y] + 1.0);
    // manually blend the background color with white to create a gradient effect
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}


// ---------------------------- Hybrid Rendering ---------------------------- //

void CameraRig::hybrid(Scene& s, Interval shutter_event) {

<<<<<<< HEAD
=======
    for (int i = 0; i < s.objects.instances.size(); i++) {
        s.objects[i]->rasterize(s_size, view, proj_to_cam, depth_buffer, color_buffer);
    }
>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)
}

void CameraRig::hybrid_worker(Scene& s, int chunk_size, int s_x, int s_y, Interval shutter_event) {
    // TODO
}


// ---------------------------- Depth Buffer ---------------------------- //



// Impure
// TODO: Incorporate chunking
void CameraRig::depth_buffer(Scene& s, Interval shutter_event) {
    // Setup
    int w = s.controls.img_w();
    int h = s.controls.img_h();

    // Allocate and initialize the buffers
    std::vector<double> depth_buffer(static_cast<size_t>(w) * h);
    std::vector<rgb> color_buffer(static_cast<size_t>(w) * h);
    
    // Intialize the buffers to infinity/black
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            depth_buffer[i + j * w] = INFINITY;
            color_buffer[i + j * w] = rgb(0,0,0);
        }
    }

    // Depth Buffer and Rasterization Algorithms
    rasterize_scene(s, depth_buffer.data(), color_buffer.data());

    // Complete! Write to image.
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            write_color(color_buffer[i + j * w], i, j);
        }
    }

    // Drop the buffers
}

void CameraRig::depth_buffer_worker(Scene& s, int chunk_size, int s_x, int s_y, Interval shutter_event) {
    // TODO
}

// Pure (can be split into chunks)
void rasterize_scene(Scene& s, double* depth_buffer, rgb* color_buffer) {
    int s_size[2];
    s_size[0] = s.controls.img_w();
    s_size[1] = s.controls.img_h();

    // Calculate transformation matricies
    mat<4,4> view = viewport_matrix(s_size[0], s_size[1]);
    mat<4,4> projection = projection_matrix(2, 10, 5, -5, -8, 8);
    mat<4,4> world_to_cam = world_to_camera_matrix(s.controls.u(), s.controls.v(), s.controls.w(), s.controls.center());
    mat<4,4> proj_to_cam = projection * world_to_cam;

<<<<<<< HEAD
    for (int i = 0; i < s.objects.instances.size(); i++) {
        s.objects[i]->rasterize(s_size, view, proj_to_cam, depth_buffer, color_buffer);
    }
}

// Impure
// TODO: Incorporate chunking
void CameraRig::depth_buffer(Scene& s) {
    // Setup
    int w = s.controls.img_w();
    int h = s.controls.img_h();

    // Allocate and initialize the buffers
    std::vector<double> depth_buffer(static_cast<size_t>(w) * h);
    std::vector<rgb> color_buffer(static_cast<size_t>(w) * h);
    
    // Intialize the buffers to infinity/black
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            depth_buffer[i + j * w] = INFINITY;
            color_buffer[i + j * w] = rgb(0,0,0);
        }
    }

    // Depth Buffer and Rasterization Algorithms
    rasterize_scene(s, depth_buffer.data(), color_buffer.data());

    // Complete! Write to image.
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            write_color(color_buffer[i + j * w], i, j);
        }
    }

    // Drop the buffers
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

>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)
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
                    break;


            }
        }
    }

    if (s.controls.mode == CameraMode::DEPTH_BUFFER) {
        depth_buffer(s);
    } else {
        tm.dispatch(tasks);
    }
}

void CameraRig::write_color(const rgb& rgb, int x, int y) {
    if (img_buffer) (*img_buffer)[y][x] = png::rgb_pixel(rgb.r, rgb.g, rgb.b);
}

color CameraRig::process_ray(const Ray& r, int depth, Scene& s) {
    if (s.controls.do_lighting) {
        return process_ray_with_lighting(r, depth, s);
    } else {
        return process_ray_without_lighting(r, depth, s);
>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)
    }
}




// ---------------------------- Camera Utilities ---------------------------- //

// (Pure) Find a random offset for caluclating anti-aliasing
point CameraRig::anti_aliasing_offset() const {
    point res;

    res[P] = 0;
    res[X] = random_double() - 0.5;
    res[Y] = random_double() - 0.5;
    res[Z] = 0;

    return res;
}

// (Pure) Generate a random point on the camera lens (for defocus blur / depth of field)
point CameraRig::random_point_on_camera_lens(CamControls& controls) const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk<4>();
    return controls.center() + (p[X] * controls.defocus_radius_u()) + (p[Y] * controls.defocus_radius_v());
}

// (Impure) Output a color to a particular pixel on the image buffer 
void CameraRig::write_color(const rgb& rgb, int x, int y) {
    if (img_buffer) (*img_buffer)[y][x] = png::rgb_pixel(rgb.r, rgb.g, rgb.b);
}

// ---------------------------------------------------------------------------- //
