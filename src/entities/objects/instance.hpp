#pragma once

#include "../../main.hpp"

class Motion;

/**
 * @brief Represents an object with an assigned position, and a list of motions
 * that are applied to the object based on the moment in time
 */
class Instance : public CollisionObject {

    public:

    std::shared_ptr<CollisionObject> obj;
    std::shared_ptr<Surface> sur;
    std::shared_ptr<mat<4,4>> loc;
    std::vector<std::shared_ptr<Motion>> mov;

    Instance(std::shared_ptr<CollisionObject> obj, std::shared_ptr<Surface> sur) : CollisionObject(sur), obj(obj), sur(sur) {}

    /**
     * @brief Pass the hit call to the original object
     */
    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        return obj->hit(r, ray_t, rec);
    }

    /**
     * @brief Pass the tcoords call to the original object
     */
    point2D get_tcoords(const point& p, double time) const override {
        return obj->get_tcoords(p, time);
    }

    /**
     * @brief Pass the raster call to the original object
     */
    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff, double time) const override {
        obj->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff, time);
    }

};

/**
 * @brief A wrapper for a vector of instances, which is itself a collision object
 * and so can be used to represent one object, composed of multiple instances.
 */
class InstanceList : public CollisionObject {

    public:
    std::vector<std::shared_ptr<CollisionObject>> instances;

    InstanceList() : CollisionObject(nullptr) {}
    InstanceList(std::shared_ptr<Surface> surf) : CollisionObject(surf) {}

    void add(const Instance& instance) {
        instances.push_back(std::make_shared<Instance>(instance));
    }

    void add(std::shared_ptr<CollisionObject> object) {
        instances.push_back(std::move(object));
    }

    /**
     * @brief Pass a hit call onto each child instance
     */
    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        bool hit_anything = false;
        double closest_so_far = ray_t.max;

        for (const auto& instance : instances) {
            if (instance->hit(r, Interval(ray_t.min, closest_so_far), rec)) {
                hit_anything = true;
                closest_so_far = rec.t;
            }
        }

        return hit_anything;
    }

    /**
     * @brief Default implementation that returns [0, 0]
     */
    point2D get_tcoords(const point&, double) const override {
        // This function should be implemented based on how you want to handle texture coordinates for the instance list.
        // For now, can return a default value or throw an exception.
        return point2D();
    }

    /**
     * @brief Pass a rasterize call onto the child instances
     */
    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff, double time) const override {
        for (const auto& instance : instances) {
            instance->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff, time);
        }
    }

    /**
     * @brief Allow accessing indexing into the child instances
     */
    std::shared_ptr<CollisionObject>& operator[](size_t index) {
        return instances[index];
    }

};
