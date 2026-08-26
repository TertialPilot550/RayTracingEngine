#pragma once

#include "ray.hpp"
#include "../lead.hpp"
#include "interval.hpp"
class Material;
class CollisionRecord {
    public:
        point p;        // point of intersection
        vec3 normal;    // normal vector at the intersection
        shared_ptr<Material> mat; // pointer to the material of the object hit
        double t;       // time of collision
        bool front_face;

        void set_face_normal(const Ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class CollisionObject {
    public:

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;

};

class CollisionList : public CollisionObject {
  public:
    std::vector<std::shared_ptr<CollisionObject>> objects;

    CollisionList() {}
    CollisionList(std::shared_ptr<CollisionObject> object) { add(object); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<CollisionObject> object) {
        objects.push_back(object);
    }

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        CollisionRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

