#pragma once

#include "../../main.hpp"

/**
 * @brief Sphere Primitive
 */
class Sphere : public CollisionObject {
    public:

        Sphere(const point& center, double radius, std::shared_ptr<Material> material) :  CollisionObject(material), center(center), radius(std::fmax(0,radius)) {}

        bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
            if (is_at_infinity(center)) return false; // If the sphere is at infinity, don't render it
            point hcenter = homogenize(center);

            point oc = hcenter - r.origin();
            auto a = norm_squared(as_vector(r.direction()));
            auto h = dot(as_vector(r.direction()), oc);
            auto c = norm_squared(oc) - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root) {
                root = (h + sqrtd) / a;
                if (root <= ray_t.min || ray_t.max <= root)
                    return false;
            }

            // HIT! Fill out the record
            point outward_normal = (r.at(root)- hcenter) / radius;
            rec.record(r, root, outward_normal, this);
            return true;
        }

        point2D get_tcoords(const point& p) const override {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = std::acos(-p[Y]);
            auto phi = std::atan2(-p[Z], p[X]) + pi;
            point2D res;
            res[0] = phi / (2*pi);
            res[1] = theta / pi;
            return res;
        }

    point center;
    double radius;
};