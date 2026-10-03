#pragma once

#include "../../../main.hpp"
#include <array>

template <int N>
class GeometricPrimitive : public CollisionObject {

    public:
    /**
     * @brief Wack ass constructor because I was getting tons of errors
     */
    template<typename... Args>
    GeometricPrimitive(std::shared_ptr<Surface> surface, Args&&... args) : CollisionObject(surface), skeleton{} {
        static_assert(sizeof...(Args) == N, "primitive requires N points");
        assign_skeleton<0>(std::forward<Args>(args)...);
    }

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff, double time) const = 0;
    virtual point2D get_tcoords(const point& p, double time) const = 0;

    std::array<point, N> skeleton;
    Motion motion;

    void set_motion(const Motion& value) { motion = value; }

protected:

    /**
     * @brief Assignment helper for the wack ass constructor
     */
    template <size_t Index, typename First, typename... Rest>
    void assign_skeleton(First&& first, Rest&&... rest) {
        // Set the first element
        skeleton[Index] = std::forward<First>(first);
        // Set the rest if you can
        if constexpr (sizeof...(Rest) > 0) {
            assign_skeleton<Index + 1>(std::forward<Rest>(rest)...);
        }
    }

    /**
     * @brief Move the point at a given index in the skeleton using the provided time
     */
    point moved_point(int index, double time) const {
        return motion.transform(skeleton[index], time);
    }

};
