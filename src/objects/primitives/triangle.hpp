#pragma once
#include "../../main.hpp"

/**
 * @brief Vertex class used by the triangle primives
 */
class Vertex {
    public:

    Vertex() {}
    Vertex(point p, point2D t_coords) : p(p), t_coords(t_coords) {}

    point p;
    point2D t_coords;

    void operator=(Vertex& v) {
        p = v.p;
        t_coords = v.t_coords;
    }

    static point2D vertex_0() {
        return point2D(0, 0);
    }

    static point2D vertex_1() {
        return point2D(0, 1);
    }

    static point2D vertex_2() {
        return point2D(1, 0.5);
    }

};

/**
 * @brief Triangle Primitive
 */
class Triangle : public CollisionObject {

    public:
    Vertex v[3];

    Triangle(point p1, point p2, point p3, std::shared_ptr<Material> mat, point2D t1 = Vertex::vertex_0(), point2D t2 = Vertex::vertex_1(), point2D t3 = Vertex::vertex_2()) : CollisionObject(mat) {
        v[0].p = p1;
        v[1].p = p2;
        v[2].p = p3;
        
        v[0].t_coords = t1;
        v[1].t_coords = t2;
        v[2].t_coords = t3;
    } 

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {

        // If any vertex is at infinity, there is not intersection
        if (is_at_infinity(v[0].p) || is_at_infinity(v[1].p) || is_at_infinity(v[2].p)) return false;
        point p0 = homogenize(v[0].p);
        point p1 = homogenize(v[1].p);
        point p2 = homogenize(v[2].p);

        const double epsilon = 1e-8; // Handle near parralel intersections to make up for doubles nonsense

        // Find the edges of the triangle as vectors
        point e1 = p1 - p0;
        point e2 = p2 - p0;

        // Vector representation of intersection candidate
        point pvec = cross(r.direction(), e2);

        // Determine whether the ray is sufficiently near parralel
        double det = dot(e1, pvec);
        if (std::fabs(det) < epsilon) return false;
        double inv_det = 1.0 / det; // for fractions

        // Ray is not parrelel. Does it intersect?

        // A point inside the triangle can be represented as P = A + u*e1 + v*e2 : u >= 0, v >= 0, u + v <= 1

        point tvec = r.origin() - p0;
        double u = dot(tvec, pvec) * inv_det;
        point qvec = cross(tvec, e1);
        double v = dot(r.direction(), qvec) * inv_det;
        bool intersection = u >= 0 && v >= 0 && u + v <= 1;
        if (!intersection) return false;

        // Now we need to determine if the intersection is within the interval of the ray 

        double t = dot(e2, qvec) * inv_det;
        bool intersects_in_range = ray_t.contains(t);
        if (!intersects_in_range) return false;
        

        // There is an intersection! Populate rec.

        point normal = cross(e1, e2);
        normal /= norm(normal);

        rec.record(r, t, normal, this);
        return true;

    }

    point2D get_tcoords(const point& p) const override {

        float d = (v[1].p[1]-v[2].p[1])*(v[0].p[0]-v[2].p[0]) + (v[2].p[1]-v[1].p[1])*(v[0].p[1]-v[2].p[1]);
        d = 1 / d;

        float l0 = d * ((v[1].p[1]-v[2].p[1])*(p[0] - v[2].p[0])+(v[2].p[0]-v[1].p[0])*(p[1]-v[2].p[1]));
        float l1 = d * ((v[2].p[1]-v[0].p[1])*(p[0] - v[2].p[0])+(v[0].p[0]-v[2].p[0])*(p[1]-v[2].p[1]));
        float l2 = 1-l0-l1;

        return l0*v[0].t_coords+l1*v[1].t_coords+l2*v[2].t_coords;

    }

};