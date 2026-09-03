#pragma once

#include "../../main.hpp"

template <int N>
class GeometricPrimitive : public CollisionObject {

    public:

    template<typename... Args>
    GeometricPrimitive(std::shared_ptr<Surface> surface, Args&&... args) : surface(surface), skeleton(std::forward<Args>(args)...) {}

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(Matrix<4,4>& viewport_matrix, Matrix<4,4>& projection_to_camera_matrix, double* depth_buff, color* color_buff);
    virtual point2D get_tcoords(const point& p) const = 0;

    std::shared_ptr<Surface> surface;
    point skeleton[N];


};