#include "lead.hpp"
#include "visual/task_master.hpp"


int main() {

    DemoScene d;

    CamControls c;
    c.set_image_width(1200);
    c.set_samples_per_pixel(500);
    c.set_image_width(1200);
    c.set_vertical_fov(20);
    c.set_camera_pos(point(13,2,3), point(0,0,0), vec3(0,1,0));
    c.set_defocus_angle(0.6);
    c.set_focus_distance(10.0);
    c.max_depth = 50;
    CameraRig cam(c);


    
    png::image<png::rgb_pixel> image = cam.capture(d);
    image.write("output.png");

    return 0;

}