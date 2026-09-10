#pragma once

#include "../../../main.hpp"
#include <array>

template <int N>
class GeometricPrimitive : public CollisionObject {

    public:

    template<typename... Args>
    GeometricPrimitive(std::shared_ptr<Surface> surface, Args&&... args)
        : CollisionObject(surface),
          skeleton{} {
        static_assert(sizeof...(Args) == N, "primitive requires N points");
        assign_skeleton<0>(std::forward<Args>(args)...);
    }

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(int screen_size[2], mat<4,4>& viewport_matrix,
                           mat<4,4>& projection_to_camera_matrix,
                           double* depth_buff, rgb* color_buff, double time) const = 0;
    virtual point2D get_tcoords(const point& p) const = 0;

    std::array<point, N> skeleton;
    Motion motion;

    void set_motion(const Motion& value) { motion = value; }

protected:
    template <size_t Index, typename First, typename... Rest>
    void assign_skeleton(First&& first, Rest&&... rest) {
        skeleton[Index] = std::forward<First>(first);
        if constexpr (sizeof...(Rest) > 0) {
            assign_skeleton<Index + 1>(std::forward<Rest>(rest)...);
        }
    }

    point moved_point(int index, double time) const {
        return motion.transform(skeleton[index], time);
    }

};

struct RasterVertex {
    point screen;
};

template <typename Shade>
inline void rasterize_triangle(const RasterVertex& a, const RasterVertex& b,
                               const RasterVertex& c, int screen_width,
                               int screen_height, double* depth_buffer,
                               rgb* color_buffer, Shade shade) {
    const double area = (b.screen[X] - a.screen[X]) *
                            (c.screen[Y] - a.screen[Y]) -
                        (b.screen[Y] - a.screen[Y]) *
                            (c.screen[X] - a.screen[X]);
    if (std::fabs(area) < 1e-12) return;

    const int min_x = std::max(0, static_cast<int>(std::floor(
        std::min({a.screen[X], b.screen[X], c.screen[X]}))));
    const int max_x = std::min(screen_width - 1, static_cast<int>(std::ceil(
        std::max({a.screen[X], b.screen[X], c.screen[X]}))));
    const int min_y = std::max(0, static_cast<int>(std::floor(
        std::min({a.screen[Y], b.screen[Y], c.screen[Y]}))));
    const int max_y = std::min(screen_height - 1, static_cast<int>(std::ceil(
        std::max({a.screen[Y], b.screen[Y], c.screen[Y]}))));

    auto edge = [](const point& p0, const point& p1, double x, double y) {
        return (x - p0[X]) * (p1[Y] - p0[Y]) -
               (y - p0[Y]) * (p1[X] - p0[X]);
    };
    auto top_left = [](const point& p0, const point& p1) {
        return p0[Y] < p1[Y] || (p0[Y] == p1[Y] && p0[X] > p1[X]);
    };
    const bool positive = area > 0;
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            const double px = x + 0.5;
            const double py = y + 0.5;
            double w0 = edge(b.screen, c.screen, px, py) / area;
            double w1 = edge(c.screen, a.screen, px, py) / area;
            double w2 = edge(a.screen, b.screen, px, py) / area;
            bool inside = true;
            if (!positive) {
                w0 = -w0;
                w1 = -w1;
                w2 = -w2;
                inside = (w0 > 0 || (std::fabs(w0) <= 1e-10 &&
                                     top_left(c.screen, b.screen))) &&
                         (w1 > 0 || (std::fabs(w1) <= 1e-10 &&
                                     top_left(a.screen, c.screen))) &&
                         (w2 > 0 || (std::fabs(w2) <= 1e-10 &&
                                     top_left(b.screen, a.screen)));
            } else {
                inside = (w0 > 0 || (std::fabs(w0) <= 1e-10 &&
                                     top_left(b.screen, c.screen))) &&
                         (w1 > 0 || (std::fabs(w1) <= 1e-10 &&
                                     top_left(c.screen, a.screen))) &&
                         (w2 > 0 || (std::fabs(w2) <= 1e-10 &&
                                     top_left(a.screen, b.screen)));
            }
            if (!inside) continue;

            const double depth = w0 * a.screen[Z] + w1 * b.screen[Z] +
                                 w2 * c.screen[Z];
            const int index = x + y * screen_width;
            if (depth < depth_buffer[index]) {
                depth_buffer[index] = depth;
                color_buffer[index] = color_correction_to_rgb(shade(
                    w0, w1, w2, w0 * a.screen[P] +
                    w1 * b.screen[P] + w2 * c.screen[P]));
            }
        }
    }
}