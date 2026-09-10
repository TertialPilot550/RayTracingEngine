#pragma once
#include "../../../main.hpp"

/**
 * @brief Parallelogram Primitive
 */
class Quad : public GeometricPrimitive<1> {
   public:
   Quad(const point& p, const point& u, const point& v, std::shared_ptr<Material> mat)
       : GeometricPrimitive<1>(mat, p), u(as_vector(u)), v(as_vector(v)) {
       auto n = cross(this->u, this->v);
       normal = unit_vector(n);
       D = dot(normal, as_point(p));
       w = n / dot(n,n);
   }

   bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
       if (is_at_infinity(skeleton[0])) return false;

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
    }

  private:
   point u, v;
   point w;
   point normal;
   double D;
};