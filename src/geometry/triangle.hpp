
#include "../lead.hpp"

class Triangle : public CollisionObject {

    public:
    point p1, p2, p3;
    std::shared_ptr<Material> mat;

    Triangle(point p1, point p2, point p3, std::shared_ptr<Material> mat) : p1(p1), p2(p2), p3(p3), mat(mat) {} 

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const {
     
        const double epsilon = 1e-8; // Handle near parralel intersections to make up for doubles nonsense

        // Find the edges of the triangle as vectors
        vec3 e1 = p2 - p1;
        vec3 e2 = p3 - p1;

        // Vector representation of intersection candidate
        vec3 pvec = cross(r.direction(), e2);

        // Determine whether the ray is sufficiently near parralel
        double det = dot(e1, pvec);
        if (std::fabs(det) < epsilon) return false;
        double inv_det = 1.0 / det; // for fractions

        // Ray is not parrelel. Does it intersect?

        // A point inside the triangle can be represented as P = A + u*e1 + v*e2 : u >= 0, v >= 0, u + v <= 1

        vec3 tvec = r.origin() - p1;
        double u = dot(tvec, pvec) * inv_det;
        vec3 qvec = cross(tvec, e1);
        double v = dot(r.direction(), qvec) * inv_det;
        bool intersection = u >= 0 && v >= 0 && u + v <= 1;
        if (!intersection) return false;

        // Now we need to determine if the intersection is within the interval of the ray 

        double t = dot(e2, qvec) * inv_det;
        bool intersects_in_range = ray_t.contains(t);
        if (!intersects_in_range) return false;
        

        // There is an intersection! Populate rec.

        vec3 normal = cross(e1, e2);
        normal /= normal.length();

        rec.t = t;
        rec.p = r.origin() + t * r.direction();
        rec.normal = normal;

        return true;

    }


};