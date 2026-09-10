#pragma once
#include "../../../main.hpp"

class Quad : public GeometricPrimitive<1> {
public:
    Quad(const point& p, const point& u, const point& v,
         std::shared_ptr<Surface> surface)
        : GeometricPrimitive<1>(surface, p), u(as_vector(u)), v(as_vector(v)) {}

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        const point origin = moved_point(0, r.time());
        const point edge_u = as_vector(motion.transform(as_point(u), r.time()));
        const point edge_v = as_vector(motion.transform(as_point(v), r.time()));
        const point n = cross(edge_u, edge_v);
        const double n_length = norm(n);
        if (n_length < 1e-12) return false;
        const point normal = n / n_length;
        const double denom = dot(normal, r.direction());
        if (std::fabs(denom) < 1e-8) return false;
        const double t = (dot(normal, origin) - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) return false;

<<<<<<< HEAD
       auto denom = dot(normal, r.direction());
       if (std::fabs(denom) < 1e-8)
           return false;

       auto t = (D - dot(normal, as_point(r.origin()))) / denom;
       if (!ray_t.contains(t))
           return false;

       auto hit_point = r.at(t);
       auto q = hit_point - skeleton[0];

       auto a = dot(q, u) / dot(u, u);
       auto b = dot(q, v) / dot(v, v);
       if (a < 0 || a > 1 || b < 0 || b > 1)
           return false;

       rec.record(r, t, normal, this);
       return true;
   }

   point2D get_tcoords(const point& p) const override {
       auto q = p - skeleton[0];
       auto a = dot(q, u) / dot(u, u);
       auto b = dot(q, v) / dot(v, v);
       return point2D(a, b);
   }

   template<int N, int M>
   void rasterize(mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, color* color_buff) {

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
=======
        const point q = r.at(t) - origin;
        const double uu = dot(edge_u, edge_u);
        const double vv = dot(edge_v, edge_v);
        const double a = dot(q, edge_u) / uu;
        const double b = dot(q, edge_v) / vv;
        if (a < 0.0 || a > 1.0 || b < 0.0 || b > 1.0) return false;
        rec.record(r, t, normal, this);
        return true;
>>>>>>> 7c439a1 (Agent Host changes for agents/time-based-ray-tracing-refactor)
    }

    point2D get_tcoords(const point& p) const override {
        const point q = p - skeleton[0];
        return point2D(dot(q, u) / dot(u, u), dot(q, v) / dot(v, v));
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport,
                   mat<4,4>& projection, double* depth_buffer,
                   rgb* color_buffer) const override {
        const point p0 = moved_point(0, 0.5);
        const point p1 = p0 + motion.transform(as_point(u), 0.5);
        const point p3 = p0 + motion.transform(as_point(v), 0.5);
        const point p2 = p1 + motion.transform(as_point(v), 0.5);
        Triangle first(p0, p1, p2, surf);
        Triangle second(p0, p2, p3, surf);
        first.rasterize(screen_size, viewport, projection, depth_buffer, color_buffer);
        second.rasterize(screen_size, viewport, projection, depth_buffer, color_buffer);
    }

private:
    point u;
    point v;
};
