#pragma once
#include "../../main.hpp"

/**
 * @brief Parallelogram Primitive
 */
class Quad : public CollisionObject {
    public:
    Quad(const point& p, const point& u, const point& v, std::shared_ptr<Material> mat): CollisionObject(mat), p(p), u(u), v(v) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, p);
        w = n / dot(n,n);
    }

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        if (is_at_infinity(p) || is_at_infinity(u) || is_at_infinity(v)) return false; // If the sphere is at infinity, don't render it
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8)
            return false;

        point hp = homogenize(p);
        point hu = homogenize(u);
        point hv = homogenize(v);

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);

        point2D t_coords = get_tcoords(r.at(t));
        
        if (!Interval(0, 1).contains(t_coords[0]) || !Interval(0, 1).contains(t_coords[1])) return false;

        
        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.record(r, t, normal, this);

        return true;
    }

    point2D get_tcoords(const point& p) const override {
        point p_vector = p - this->p;
        float a = dot(w, cross(p_vector, v));
        float b = dot(w, cross(u, p_vector));
        return point2D(a, b);
    }

  private:
    point p;
    point u, v;
    point w;
    point normal;
    double D;
};