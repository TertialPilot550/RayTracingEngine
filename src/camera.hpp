#include <png++/png.hpp>

#include "lead.hpp"
#include "scene.hpp"

using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

class Camera {

    public:

    // Image Parameters
    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 10;

    Image* last_image;

    Image& capture(Scene &s) {
        initialize();
        return render(s);
    }

    private: 

    int image_height = 0;
    double pixels_samples_scale;

    vec3 pixel_delta_u;
    vec3 pixel_delta_v;

    point camera_center;
    point pixel00_loc;

    void initialize() {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        if (last_image) delete last_image;
        last_image = new Image(image_width, image_height);

        pixels_samples_scale = 1.0 / samples_per_pixel;
        // Camera
        double focal_length = 1.0;
        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(image_width)/image_height);
        camera_center = point(0, 0, 0);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        point viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    Ray get_ray(int i, int j) const {
        auto offset = sample_square(); // this is the anti aliasing part. set to 0.5 to turn it off
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;
        return Ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

  
    Image& render(Scene& s) {
        if (last_image) delete last_image;
        last_image = new Image(image_height, image_width);
        // std::cout << "Rendering Image of Size: [" << image_width << ", " << image_height << "]\n";
        for (int y = 0; y < last_image->get_height(); y++) {
            std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
            for (int x = 0; x < last_image->get_width(); x++) {


                color pixel_color(0, 0, 0);
                for (int i = 0; i < samples_per_pixel; i++) {
                    Ray r = get_ray(x, y);
                    pixel_color += process_ray(r, s.objects);
                }

                Interval unit = Interval(0, 1);

                int r1 = unit.clamp(pixel_color[0])*256;
                int r2 = unit.clamp(pixel_color[1])*256;
                int r3 = unit.clamp(pixel_color[2])*256;

                (*last_image)[y][x] = png::rgb_pixel(r1, r2, r3); // Blue
            }
        }

        return *last_image;

    }

    color process_ray(const Ray& r, CollisionList& objects) {
        CollisionRecord rec;
        if (objects.hit(r, Interval(0, infinity), rec)) {
            return 0.5 * (rec.normal + color(1,1,1));
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

};