#pragma once

#include "../../main.hpp"

class Material;
class Surface;
class CollisionRecord;

/**
 * @brief Abstract class representing a collidable object.
 */
 class CollisionObject {
    public:
    std::shared_ptr<Surface> surf;
    CollisionObject(std::shared_ptr<Surface> surf): surf(surf) {}

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const {}
    virtual point2D get_tcoords(const point& p) const = 0;

};

/**
 * @brief Track collisions
 * 
 * @details Used to handle collisions seperately from intersections. 
 */
class CollisionRecord {
    public:
        point p;        // point of intersection
        point normal;    // normal vector at the intersection
        std::shared_ptr<Surface> surf; // pointer to the material of the object hit
        double t;       // time of collision
        point2D t_coords;
        bool front_face;

        /**
         * @brief Automatically record collision details given the collision intersection time, normal, and colliding object
         */
        void record(const Ray&r, float t, const point& outward_normal, const CollisionObject* obj) {
            this->t = t;
            this->p = r.at(t);
            set_face_normal(r, outward_normal);
            point2D t_c = obj->get_tcoords(p);
            this->t_coords = t_c;
            this->surf = obj->surf;
        }

        void set_face_normal(const Ray& r, const point& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot<4>(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }

        void operator=(CollisionRecord& r) {
            p = r.p;
            normal = r.normal;
            surf = r.surf;
            t = r.t;
            t_coords = r.t_coords;
            front_face = r.front_face;
        }
};


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

    point2D get_tcoords(const point& p) const override {
        point2D res;
        res[0] = 0;
        res[1] = 0;
        return res;
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

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const override {
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff);
        }
    }

};

