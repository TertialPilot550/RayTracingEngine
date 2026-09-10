#pragma once

#include "../../main.hpp"

class Motion {

    public:
    mat<4,4> start, end;
    Interval time;

    // rough, linear interpolation; might want to redo this
    mat<4,4> at(double t) {
        double len = time.max-time.min;
        if (len == 0) len = 1;
        double p = t / (time.max-time.min);
        Interval(0, 1).clamp(p);
        return (1-p)*start + end*p;
    }

    vec<4> transform(const vec<4> v, double t) {
        return at(t) * v;
    }

};

class Instance : public CollisionObject {

    public:

    std::shared_ptr<CollisionObject> obj;
    std::shared_ptr<Surface> sur;
    std::shared_ptr<mat<4,4>> loc;
    std::vector<std::shared_ptr<Motion>> mov;

    Instance(std::shared_ptr<CollisionObject> obj, std::shared_ptr<Surface> sur) : CollisionObject(sur), obj(obj), sur(sur) {}

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        return obj->hit(r, ray_t, rec);
    }

    point2D get_tcoords(const point& p) const override {
        return obj->get_tcoords(p);
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const {
        obj->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff);
    }

};

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

    point2D get_tcoords(const point& p) const override {
        // This function should be implemented based on how you want to handle texture coordinates for the instance list.
        // For now, we can return a default value or throw an exception.
        return point2D();
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const override {
        for (const auto& instance : instances) {
            instance->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff);
        }
    }

    std::shared_ptr<CollisionObject>& operator[](size_t index) {
        return instances[index];
    }

};
