#pragma once

#include "../main.hpp"

class Material;
class CollisionObject;

/**
 * @brief Track collisions
 * 
 * @details Used to handle collisions seperately from intersections. 
 */
class CollisionRecord {
    public:
        point p;        // point of intersection
        vec3 normal;    // normal vector at the intersection
        std::shared_ptr<Material> mat; // pointer to the material of the object hit
        double t;       // time of collision
        vec2 t_coords;
        bool front_face;

        void record(const Ray&r, float t, const vec3& outward_normal, const CollisionObject* obj);

        void set_face_normal(const Ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }

        void operator=(CollisionRecord& r) {
            p = r.p;
            normal = r.normal;
            mat = r.mat;
            t = r.t;
            t_coords = r.t_coords;
            front_face = r.front_face;
        }
};

/**
 * @brief Abstract class representing a collidable object.
 */
 class CollisionObject {
    public:
    std::shared_ptr<Material> mat;
    CollisionObject(std::shared_ptr<Material> m): mat(m) {}

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual vec2 get_tcoords(const point& p) const = 0;

};

/**
 * @brief Automatically record collision details given the collision intersection time, normal, and colliding object
 */
inline void CollisionRecord::record(const Ray&r, float t, const vec3& outward_normal, const CollisionObject* obj) {
    this->t = t;
    this->p = r.at(t);
    set_face_normal(r, outward_normal);
    vec2 t_coords = obj->get_tcoords(p);
    this->t_coords = t_coords;
    this->mat = obj->mat;
}

/**
 * @brief A list of collidable objects
 * @details Used to create collections of primitives that function as a single object
 */
class CollisionList : public CollisionObject {
  public:
    std::vector<std::shared_ptr<CollisionObject>> objects;

    CollisionList() : CollisionObject(NULL) {}
    CollisionList(std::shared_ptr<CollisionObject> object) : CollisionObject(NULL) { add(object); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<CollisionObject> object) {
        objects.push_back(object);
    }

    vec2 get_tcoords(const point& p) const override {
        return vec2(0,0);
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

