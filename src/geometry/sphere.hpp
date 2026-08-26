#pragma once

#include "../lead.hpp"

class Sphere : public CollisionObject {
    public:

        Sphere(const point& center, double radius, std::shared_ptr<Material> material) : center(center), radius(std::fmax(0,radius)), mat(material) {
        }

        bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

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

            rec.t = root;
            rec.p = r.at(rec.t);

            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);

            rec.mat = mat;

            return true;
        }

    point center;
    double radius;
    shared_ptr<Material> mat; // pointer to the material of the object
};