#pragma once

#include "ray.hpp"
#include "../lead.hpp"

class CollisionRecord {
    public:
        point p;        // point of intersection
        vec3 normal;    // normal vector at the intersection
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

    virtual ~CollisionObject() = default;

    virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, CollisionRecord& rec) const = 0;

};

class CollisionList : public CollisionObject {
  public:
    std::vector<shared_ptr<CollisionObject>> objects;

    CollisionList() {}
    CollisionList(std::shared_ptr<CollisionObject> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<CollisionObject> object) {
        objects.push_back(object);
    }

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, CollisionRecord& rec) const override {
        CollisionRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : objects) {
            if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

