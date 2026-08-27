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

// Control Class that ensures certain relationships between it's parameters
class CamControls {

    private: 

    double aspect_ratio = 16.0/9;
    int image_width = 400;
    int samples_per_pixel = 10;
    double vertical_fov = 90;
    double defocus_angle = 0;
    double focus_dist = 10;

    int image_height = 0;
    double pixels_samples_scale = 1.0 / samples_per_pixel;

    vec3 pixel_delta_u;
    vec3 pixel_delta_v;

    point camera_center;
    point facing = point(0, 0, -1);
    vec3 camera_up = vec3(0, 1, 0);
    point pixel00_loc;

    vec3 b_u = vec3(0, 0, 0); // camera frame basis vectors
    vec3 b_v = vec3(0, 0, 0); // camera frame basis vectors
    vec3 b_w = vec3(0, 0, 0); // camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    vec3 viewport_u;
    vec3 viewport_v;

    public:

    int max_depth = 50;
    double gamma = 0.6;
    int chunk_size = image_width;
    int thread_count = 1;

    CamControls() {
        update();
    }

    void set_camera_pos(point p, point f, point u) {
        camera_center = p;
        facing = f;
        camera_up = u;
        update();
    }

    void set_aspect_ratio(double r) {
        aspect_ratio = r;
        update();
    }

    void set_image_width(int w) {
        image_width = w;
        update();
    }

    void set_vertical_fov(double f) {
        vertical_fov = f;
        update();
    }

    void set_samples_per_pixel(int s) {
        samples_per_pixel = s;
        pixels_samples_scale = 1.0 / samples_per_pixel;
    }

    void set_focus_distance(double f) {
        focus_dist = f;
        update();
    }

    void set_defocus_angle(double f) {
        defocus_angle = f;
        update();
    }

    double asp_ratio() {return aspect_ratio;}
    int img_w() {return image_width;}
    int img_h() {return image_height;}
    double pscale() {return pixels_samples_scale;}
    double samples_per_pix() {return samples_per_pixel;}
    double vfov() {return vertical_fov;}
    double defcs_angle() {return defocus_angle;}
    double focus_d() {return focus_dist;}

    vec3 du() {return pixel_delta_u;}
    vec3 dv() {return pixel_delta_v;}

    point center() {return camera_center;}
    point face() {return facing;}
    vec3 up() {return camera_up;}
    vec3 viewport_origin() {return pixel00_loc;}

    vec3 u() {return b_u;}
    vec3 v() {return b_v;}
    vec3 w() {return b_w;}

    vec3 defocus_radius_u() {return defocus_disk_u;}
    vec3 defocus_radius_v() {return defocus_disk_v;}

    vec3 viewp_u() {return viewport_u;};
    vec3 viewp_v() {return viewport_v;};

    private:

    void update() {
        chunk_size = image_width;
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        
        auto theta = degrees_to_radians(vertical_fov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width)/image_height);

        b_w = unit_vector(camera_center - facing);
        b_u = unit_vector(cross(camera_up, b_w));
        b_v = cross(b_w, b_u);

        viewport_u = viewport_width * b_u;    // Vector across viewport horizontal edge
        viewport_v = viewport_height * -b_v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        auto viewport_upper_left = camera_center - (focus_dist * b_w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = b_u * defocus_radius;
        defocus_disk_v = b_v * defocus_radius;
    }

};


class CameraRig {

    public:

    CamControls& controls;
    Image* img_buffer = NULL;

    CameraRig(CamControls& controls) : controls(controls) {}

    Image& capture(Scene& s) {
        if (img_buffer) delete img_buffer;
        img_buffer = new Image(controls.img_w(), controls.img_h());
        
        render(s);

        return (*img_buffer);
    }


    private: 

    // Impure
    void render(Scene& s) {

        TaskMaster tm(controls.thread_count, controls.chunk_size);
        std::vector<std::function<void()>> tasks;

        // For each chunk...
        for (int y = 0; y < controls.img_h(); y++) {
            for (int x = 0; x < controls.img_w(); x+= controls.chunk_size) {
                // Create a function object encapsulating the computation for that chunk
                tasks.emplace_back([this, x, y, &s]() {

                    rgb res[controls.chunk_size];

                    // Calculate pixel values for each pixel in the chunk
                    for (int i = 0; i < controls.chunk_size && x+i < controls.img_w(); i++) {
                        res[i] = color_for_pixel_xy(x+i, y, s.objects);
                    }

                    // Write to buffer after to take advanatage of locality
                    // !! Impure !!
                    for (int i = 0; i < controls.chunk_size && x+i < controls.img_w(); i++) {
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
    point defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return controls.center() + (p[0] * controls.defocus_radius_u()) + (p[1] * controls.defocus_radius_v());
    }

    // Pure
    Ray get_ray_for_pixel(int i, int j) const {
        auto offset = sample_square(); // this is the anti aliasing part. set to 0.5 to turn it off
        auto pixel_sample = controls.viewport_origin() + ((i + offset.x()) * controls.du()) + ((j + offset.y()) * controls.dv());
        auto ray_origin = (controls.defcs_angle() <= 0) ? controls.center() : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return Ray(ray_origin, ray_direction);
    }

    // Pure 
    color process_ray(const Ray& r, int depth, CollisionList& objects) {
        // CollisionRecord rec;
        // if (depth <= 0) {
        //     return color(0, 0, 0);
        // }
        // if (objects.hit(r, Interval(0.001, infinity), rec)) {
        //     Ray scattered;
        //     color attenuation;
        //     if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered))
        //         return attenuation * process_ray(scattered, depth-1, objects);
        //     return color(0,0,0);
        // }
        // vec3 unit_direction = unit_vector(r.direction());
        // auto a = 0.5*(unit_direction.y() + 1.0);
        // // manually blend the background color with white to create a gradient effect
        // return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);

        volatile double x = 0;

        for (long long i = 0; i < 1000000; i++) {
            x += i * 0.000001;
        }

        return color(0, 0, 0);


    }


    // Pure
    rgb color_for_pixel_xy(int x, int y, CollisionList& objects) {
        color pixel_color(0, 0, 0);
        for (int i = 0; i < controls.samples_per_pix(); i++) {
            Ray r = get_ray_for_pixel(x, y);
            pixel_color += process_ray(r, controls.max_depth, objects);
        }
        return color_correction_to_rgb(controls.pscale() * pixel_color);
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