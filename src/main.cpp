#include "lead.hpp"
#include "visual/task_master.hpp"


int main() {

    TriangleDemo d;

    
    CameraRig cam;
    png::image<png::rgb_pixel> image = cam.capture(d);
    image.write("output.png");

    return 0;

}