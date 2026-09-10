#pragma once
#include "../../../main.hpp"

class Quad : public GeometricPrimitive<1> {
public:
    Quad(const point& p, const point& u, const point& v,
         std::shared_ptr<Surface> surface)
        : GeometricPrimitive<1>(surface, p), u(as_vector(u)), v(as_vector(v)) {}

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        const point origin = moved_point(0, r.time());
        const point edge_u = as_vector(motion.transform(as_point(u), r.time()));
        const point edge_v = as_vector(motion.transform(as_point(v), r.time()));
        const point n = cross(edge_u, edge_v);
        const double n_length = norm(n);
        if (n_length < 1e-12) return false;
        const point normal = n / n_length;
        const double denom = dot(normal, r.direction());
        if (std::fabs(denom) < 1e-8) return false;
        const double t = (dot(normal, origin) - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) return false;

        const point q = r.at(t) - origin;
        const double uu = dot(edge_u, edge_u);
        const double vv = dot(edge_v, edge_v);
        const double a = dot(q, edge_u) / uu;
        const double b = dot(q, edge_v) / vv;
        if (a < 0.0 || a > 1.0 || b < 0.0 || b > 1.0) return false;
        rec.record(r, t, normal, this);
        return true;
    }

    point2D get_tcoords(const point& p) const override {
        return get_tcoords(p, 0.0);
    }

    point2D get_tcoords(const point& p, double time) const override {
        const point origin = moved_point(0, time);
        const point edge_u = as_vector(motion.transform(as_point(u), time));
        const point edge_v = as_vector(motion.transform(as_point(v), time));
        const point q = p - origin;
        return point2D(dot(q, edge_u) / dot(edge_u, edge_u),
                      dot(q, edge_v) / dot(edge_v, edge_v));
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport,
                   mat<4,4>& projection, double* depth_buffer,
                   rgb* color_buffer, double time) const override {
        const point p0 = moved_point(0, time);
        const point p1 = p0 + motion.transform(as_point(u), time);
        const point p3 = p0 + motion.transform(as_point(v), time);
        const point p2 = p1 + motion.transform(as_point(v), time);
        Triangle first(p0, p1, p2, surf);
        Triangle second(p0, p2, p3, surf);
        first.rasterize(screen_size, viewport, projection, depth_buffer, color_buffer, time);
        second.rasterize(screen_size, viewport, projection, depth_buffer, color_buffer, time);
    }

private:
    point u;
    point v;
};
