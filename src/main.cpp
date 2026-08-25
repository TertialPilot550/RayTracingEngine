#include <png++/png.hpp>
#include "geometry/vector.hpp"
#include "geometry/ray.hpp"
#include "geometry/sphere.hpp"

color ray_color(const Ray& r, const CollisionObject& world) {
    CollisionRecord rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

png::image<png::rgb_pixel> render() {

    // Image parameters
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    png::image<png::rgb_pixel> image(image_width, image_height);

    // TODO 

    CollisionList world;

    world.add(make_shared<Sphere>(point(0,0,-1), 0.5));
    world.add(make_shared<Sphere>(point(0,-100.5,-1), 100));


    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width)/image_height);
    point camera_center = point(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    point viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    point pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::cout << "Rendering Image of Size: [" << image_width << ", " << image_height << "]\n";
    for (int y = 0; y < image.get_height(); y++) {
        std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
        for (int x = 0; x < image.get_width(); x++) {

            // Find the point on the viewing plane the ray intersects, and create a ray from the camera in the direction of that midpoint
            point pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r, world);

            int r1 = pixel_color[0]*255.999;
            int r2 = pixel_color[1]*255.999;
            int r3 = pixel_color[2]*255.999;

            std::cout << "(" << r1 << " " << r2 << " " << r3 << ")" << "\n";

            image[y][x] = png::rgb_pixel(r1, r2, r3); // Blue
        }
    }

    return image;
}

int main() {
    png::image<png::rgb_pixel> image = render();
    image.write("output.png");
    return 0;
}
