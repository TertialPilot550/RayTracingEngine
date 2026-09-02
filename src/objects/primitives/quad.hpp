#pragma once
#include "../../main.hpp"

/**
 * @brief Parallelogram Primitive
 */
class Quad : public CollisionObject {
   public:
   Quad(const point& p, const point& u, const point& v, std::shared_ptr<Material> mat)
       : CollisionObject(mat), p(p), u(as_vector(u)), v(as_vector(v)) {
       auto n = cross(this->u, this->v);
       normal = unit_vector(n);
       D = dot(normal, as_point(p));
       w = n / dot(n,n);
   }

   bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
       if (is_at_infinity(p)) return false;

       auto denom = dot(normal, r.direction());
       if (std::fabs(denom) < 1e-8)
           return false;

       auto t = (D - dot(normal, as_point(r.origin()))) / denom;
       if (!ray_t.contains(t))
           return false;

       auto hit_point = r.at(t);
       auto q = hit_point - p;

       auto a = dot(q, u) / dot(u, u);
       auto b = dot(q, v) / dot(v, v);
       if (a < 0 || a > 1 || b < 0 || b > 1)
           return false;

       rec.record(r, t, normal, this);
       return true;
   }

   point2D get_tcoords(const point& p) const override {
       auto q = p - this->p;
       auto a = dot(q, u) / dot(u, u);
       auto b = dot(q, v) / dot(v, v);
       return point2D(a, b);
   }

  private:
   point p;
   point u, v;
   point w;
   point normal;
   double D;
};