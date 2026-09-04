#pragma once

#include "../../../main.hpp"

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

    void rasterize(mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double depth_buff[N][M], color color_buff[N][M]) {

        mat<4,4> object_instance;

        // Project the object

        // Find a bounding box

        // For each pixel within the projection bounds...
        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 1; j++) {
        

                // Compute the object's transformation matrix
                
                // Perform the transformation on points

                // Rasterize method on collision object

                int z; // TODO. Let z be the depth of the pixel 
                color c; // TODO. Let c be the color of the pixel

                

                if (z < depth_buff[i][j]) {
                    depth_buff[i][j] = z;
                    color_buff[i][j] = c;
                }

            }
        }
    }

    point center;
    double radius;
};