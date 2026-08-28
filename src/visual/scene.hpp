#pragma once

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

    Scene() : controls(CamControls()) {}
    


    
};

class DemoScene : public Scene {

    public:

    DemoScene() {

        controls.set_image_width(1200);
        controls.set_samples_per_pixel(10);
        controls.set_image_width(1200);
        controls.set_vertical_fov(20);
        controls.set_camera_pos(point(13,2,3), point(0,0,0), vec3(0,1,0));
        controls.set_defocus_angle(0.6);
        controls.set_focus_distance(10.0);
        controls.max_depth = 50;




       auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
        objects.add(std::make_shared<Sphere>(point(0,-1000,0), 1000, ground_material));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = random_double();
                point center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

                if ((center - point(4, 0.2, 0)).length() > 0.9) {
                    std::shared_ptr<Material> sphere_material;

                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = color::random() * color::random();
                        sphere_material = std::make_shared<Lambertian>(albedo);
                        objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                    } else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = std::make_shared<Metal>(albedo, fuzz);
                        objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                    } else {
                        // glass
                        sphere_material = std::make_shared<Dielectric>(1.5);
                        objects.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = std::make_shared<Dielectric>(1.5);
        objects.add(std::make_shared<Sphere>(point(0, 1, 0), 1.0, material1));
    }

    

};

class TriangleDemo : public Scene {
    public:
    TriangleDemo() {
        controls.set_image_width(1200);
        controls.set_samples_per_pixel(10);
        controls.set_image_width(1200);
        controls.set_vertical_fov(20);
        controls.set_camera_pos(point(10,-50, 10), point(0,0,0), vec3(0,0,1));
        controls.set_defocus_angle(0.6);
        controls.set_focus_distance(10.0);
        controls.max_depth = 50;


        auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
        objects.add(std::make_shared<Sphere>(point(0,0,-1000), 1000, ground_material));

        for (int i = 0; i < 10; i++) {
            auto albedo = color(255.0/256, 0.0, 0.0);
            auto mat = std::make_shared<Lambertian>(albedo);
            objects.add(std::make_shared<Triangle>(point((i-5)*5, (i-5)*5, 0), point((i-5)*5+2, (i-5)*5+2, 0), point((i-5)*5+1, (i-5)*5+1, 5), mat));
        }



    }
};