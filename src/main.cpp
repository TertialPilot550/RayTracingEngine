#include "lead.hpp"


int main() {
    DemoScene d;

    Camera cam;
    


    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point(13,2,3);
    cam.lookat   = point(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    png::image<png::rgb_pixel> image = cam.capture(d);
    image.write("output.png");
    return 0;

}