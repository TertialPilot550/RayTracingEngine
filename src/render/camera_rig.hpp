#pragma once
#include "../lead.hpp"

// courtesy of png++
using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

// used for returning integer rgb values from a function
class rgb {
    public:
    
    int r;
    int g;
    int b;

    rgb(): r(0), g(0), b(0) {}

    rgb(int r, int g, int b): r(r), g(g), b(b) {}

    void operator=(rgb color) {
        r = color.r;
        g = color.g;
        b = color.b;
    }
};


class CameraRig {

    public:

    Image* img_buffer = NULL;

    CameraRig()  {}

    Image& capture(Scene& s) {
        if (img_buffer) delete img_buffer;
        img_buffer = new Image(s.controls.img_w(), s.controls.img_h());
        
        render(s);

        return (*img_buffer);
    }


    private: 

    // Impure
    void render(Scene& s) {


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
                        res[i] = color_for_pixel_xy(x+i, y, s);
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

    // Write to the image buffer (impure)
    void write_color(const rgb& rgb, int x, int y) {
        if (img_buffer) (*img_buffer)[y][x] = png::rgb_pixel(rgb.r, rgb.g, rgb.b);
    }

    // Pure 
    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    // Pure
    point defocus_disk_sample(CamControls& controls) const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return controls.center() + (p[0] * controls.defocus_radius_u()) + (p[1] * controls.defocus_radius_v());
    }

    // Pure
    Ray get_ray_for_pixel(int i, int j, CamControls& controls) const {
        auto offset = sample_square(); // this is the anti aliasing part. set to 0.5 to turn it off
        auto pixel_sample = controls.viewport_origin() + ((i + offset.x()) * controls.du()) + ((j + offset.y()) * controls.dv());
        auto ray_origin = (controls.defcs_angle() <= 0) ? controls.center() : defocus_disk_sample(controls);
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();
        return Ray(ray_origin, ray_direction, ray_time);
    }

    // Pure 
    color process_ray(const Ray& r, int depth, Scene& s) {
        CollisionRecord rec;
        if (depth <= 0) {
            return color(0, 0, 0);
        }

        // If the ray hits nothing, return the background color.
        if (!s.objects.hit(r, Interval(0.001, infinity), rec))
            return s.background;

        Ray scattered;
        color attenuation;

        if (!rec.mat) {
            auto b = std::make_shared<SolidColor>(0,0,0);
            auto p = std::make_shared<SolidColor>(247, 127, 190);

            auto check = std::make_shared<CheckerTexture>(2.0, b, p);
            std::shared_ptr<Material> m = std::make_shared<Lambertian>(check);
            rec.mat = m;
        }

        // (double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) : inv_scale(1.0 /scale), even(even), odd(odd) {}

        color color_from_emission = rec.mat->emitted(rec.t_coords.f1, rec.t_coords.f2, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * process_ray(scattered, depth-1, s);

        return color_from_emission + color_from_scatter;
    }


    // Pure
    rgb color_for_pixel_xy(int x, int y, Scene& s) {
        color pixel_color(0, 0, 0);
        for (int i = 0; i < s.controls.samples_per_pix(); i++) {
            Ray r = get_ray_for_pixel(x, y, s.controls);
            pixel_color += process_ray(r, s.controls.max_depth, s);
        }
        return color_correction_to_rgb(s.controls.pscale() * pixel_color);
    }

    // Pure
    rgb color_correction_to_rgb(const color& pixel_color) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        // std::cout << r << g << b;

        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // std::cout << r << g << b;

        // Translate the [0,1] component values to the byte range [0,255].
        static Interval unit(0, 0.999);
        int rbyte = int(256 * unit.clamp(r));
        int gbyte = int(256 * unit.clamp(g));
        int bbyte = int(256 * unit.clamp(b));

        return rgb(rbyte, gbyte, bbyte);
    }

    

};