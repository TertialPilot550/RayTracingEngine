#pragma once
#include "../../../main.hpp"

class Sphere : public CollisionObject {
public:
    Sphere(const point& center, double radius, std::shared_ptr<Surface> surface)
        : CollisionObject(surface),
          center(center), radius(std::fmax(0.0, radius)) {}

    void set_motion(const Motion& value) { motion = value; }

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        if (is_at_infinity(center)) return false;
        const point hcenter = homogenize(motion.transform(center, r.time()));
        const point oc = hcenter - r.origin();
        const double a = norm_squared(as_vector(r.direction()));
        const double half_b = dot(as_vector(r.direction()), oc);
        const double c = norm_squared(oc) - radius * radius;
        const double discriminant = half_b * half_b - a * c;
        if (discriminant < 0.0) return false;

        const double root_distance = std::sqrt(discriminant);
        double root = (half_b - root_distance) / a;
        if (!ray_t.contains(root)) {
            root = (half_b + root_distance) / a;
            if (!ray_t.contains(root)) return false;
        }
        const point outward_normal = (r.at(root) - hcenter) / radius;
        rec.record(r, root, outward_normal, this);
        return true;
    }

    point2D get_tcoords(const point& p) const override {
        const point local = p - center;
        const double theta = std::acos(std::clamp(-local[Y] / radius, -1.0, 1.0));
        const double phi = std::atan2(-local[Z], local[X]) + pi;
        return point2D(phi / (2 * pi), theta / pi);
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix,
                   mat<4,4>& projection, double* depth_buffer,
                   rgb* color_buffer) const override {
        const point c = homogenize(motion.transform(center, 0.5));
        const point px = homogenize(motion.transform(center + make_vector(radius, 0, 0), 0.5));
        const point py = homogenize(motion.transform(center + make_vector(0, radius, 0), 0.5));
        const point sc = homogenize(viewport_matrix * homogenize(projection * c));
        const point sx = homogenize(viewport_matrix * homogenize(projection * px));
        const point sy = homogenize(viewport_matrix * homogenize(projection * py));
        const double rx = std::fabs(sx[X] - sc[X]);
        const double ry = std::fabs(sy[Y] - sc[Y]);
        if (rx < 0.5 || ry < 0.5 || !surf || !surf->mat) return;

        const int min_x = std::max(0, static_cast<int>(std::floor(sc[X] - rx)));
        const int max_x = std::min(screen_size[0] - 1, static_cast<int>(std::ceil(sc[X] + rx)));
        const int min_y = std::max(0, static_cast<int>(std::floor(sc[Y] - ry)));
        const int max_y = std::min(screen_size[1] - 1, static_cast<int>(std::ceil(sc[Y] + ry)));
        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                const double dx = (x + 0.5 - sc[X]) / rx;
                const double dy = (y + 0.5 - sc[Y]) / ry;
                if (dx * dx + dy * dy > 1.0) continue;
                const int index = x + y * screen_size[0];
                if (sc[Z] < depth_buffer[index]) {
                    depth_buffer[index] = sc[Z];
                    color_buffer[index] = color_correction_to_rgb(
                        surf->mat->at(0, 0, c));
                }
            }
        }
    }

private:
    point center;
    double radius;
    Motion motion;
};
