#pragma once
#include "../../lead.hpp"

class Quad : public CollisionObject {
    public:
    Quad(const point& p, const vec3& u, const vec3& v, std::shared_ptr<Material> mat): p(p), u(u), v(v) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, p);
        w = n / dot(n,n);
        this->mat = mat;
    }

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);

        vec2 t_coords = get_tcoords(r.at(t));
        
        if (Interval(0, 1).contains(t_coords.f1) || Interval(0, 1).contains(t_coords.f2)) return false;

        
        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.record(r, t, normal, this);

        return true;
    }

    vec2 get_tcoords(const point& p) const {
        vec3 p_vector = p - this->p;
        float a = dot(w, cross(p_vector, v));
        float b = dot(w, cross(u, p_vector));
        return vec2(a, b);
    }

  private:
    point p;
    vec3 u, v;
    vec3 w;
    vec3 normal;
    double D;
};