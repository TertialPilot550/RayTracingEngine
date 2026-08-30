#pragma once
#include "../lead.hpp"
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
    int thread_count = 50;

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

class Scene {

    public:
    CamControls controls;
    CollisionList objects;
    color background;

    // Default to a kind of sky blue
    Scene() : controls(CamControls()), background(176, 252, 255) {}

    
};



