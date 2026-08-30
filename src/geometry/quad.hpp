#pragma once
#include "../lead.hpp"

class Quad : public CollisionObject {
  public:
    public:
    Quad(const point& Q, const vec3& u, const vec3& v, std::shared_ptr<Material> mat)
      : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n,n);

        // set_bounding_box();
    }

    // virtual void set_bounding_box() {
    //     // Compute the bounding box of all four vertices.
    //     auto bbox_diagonal1 = aabb(Q, Q + u + v);
    //     auto bbox_diagonal2 = aabb(Q + u, Q + v);
    //     bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    // }

    // aabb bounding_box() const override { return bbox; }

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
        vec3 p_vector = p - Q;
        float a = dot(w, cross(p_vector, v));
        float b = dot(w, cross(u, p_vector));
        return vec2(a, b);
    }

  private:
    point Q;
    vec3 u, v;
    vec3 w;
    std::shared_ptr<Material> mat;
    // aabb bbox;
    vec3 normal;
    double D;
};