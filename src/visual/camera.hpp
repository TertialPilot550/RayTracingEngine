#include <png++/png.hpp>

#include "../lead.hpp"

using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

class Camera {

    public:

    // Image Parameters
    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 10;
    int max_depth = 50; // Maximum number of bounces for a ray off of an object that reflects

    double vfov = 90;  // Vertical view angle (field of view)
    point lookfrom = point(0,0,0);   // Point camera is looking from
    point lookat   = point(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10; 

    double gamma = 0.6; // gamma like from minecraft. In [0, 1)

    Image* last_image = NULL;

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

    vec3 u, v, w; // camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius
    
    void write_color(std::ostream& out, const color& pixel_color, int x, int y) {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Translate the [0,1] component values to the byte range [0,255].
        static Interval unit(0, 0.999);
        int rbyte = int(256 * unit.clamp(r));
        int gbyte = int(256 * unit.clamp(g));
        int bbyte = int(256 * unit.clamp(b));

        

        // Write out the pixel color components.
        // out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';

        // Print to the image buffer as well.
        (*last_image)[y][x] = png::rgb_pixel(rbyte, gbyte, bbyte);
    }


    void initialize() {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        if (last_image!=NULL) delete last_image;
        last_image = new Image(image_width, image_height);

        pixels_samples_scale = 1.0 / samples_per_pixel;

        camera_center = lookfrom;
        // Camera
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width)/image_height);


        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge


        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Ray get_ray(int i, int j) const {
        auto offset = sample_square(); // this is the anti aliasing part. set to 0.5 to turn it off
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return Ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

  
    Image& render(Scene& s) {
        if (last_image) delete last_image;
        last_image = new Image(image_width, image_height);
        // std::cout << "Rendering Image of Size: [" << image_width << ", " << image_height << "]\n";
        for (int y = 0; y < last_image->get_height(); y++) {
            std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
            for (int x = 0; x < last_image->get_width(); x++) {


                color pixel_color(0, 0, 0);
                for (int i = 0; i < samples_per_pixel; i++) {
                    Ray r = get_ray(x, y);
                    pixel_color += process_ray(r, max_depth,s.objects);
                }
                write_color(std::cout, pixels_samples_scale * pixel_color, x, y);
            }
        }

        return *last_image;

    }

    color process_ray(const Ray& r, int depth, CollisionList& objects) {
        CollisionRecord rec;
        if (depth <= 0) {
            return color(0, 0, 0);
        }
        if (objects.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            color attenuation;
            if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * process_ray(scattered, depth-1, objects);
            return color(0,0,0);
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        // manually blend the background color with white to create a gradient effect
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

};