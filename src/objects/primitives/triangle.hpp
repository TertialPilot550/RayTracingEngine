#pragma once
#include "../../main.hpp"

class Vertex {
    public:

    Vertex() {}
    Vertex(point p, vec2 t_coords) : p(p), t_coords(t_coords) {}

    point p;
    vec2 t_coords;

    void operator=(Vertex& v) {
        p = v.p;
        t_coords = v.t_coords;
    }

};

class Triangle : public CollisionObject {

    public:
    Vertex v[3];

    Triangle(point p1, point p2, point p3, std::shared_ptr<Material> mat, vec2 t1 = vec2::vertex_0(), vec2 t2 = vec2::vertex_1(), vec2 t3 = vec2::vertex_2()) : CollisionObject(mat) {
        v[0].p = p1;
        v[1].p = p2;
        v[2].p = p3;
        
        v[0].t_coords = t1;
        v[1].t_coords = t2;
        v[2].t_coords = t3;
    } 

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
     
        const double epsilon = 1e-8; // Handle near parralel intersections to make up for doubles nonsense

        // Find the edges of the triangle as vectors
        vec3 e1 = v[2].p - v[1].p;
        vec3 e2 = v[3].p - v[1].p;

        // Vector representation of intersection candidate
        vec3 pvec = cross(r.direction(), e2);

        // Determine whether the ray is sufficiently near parralel
        double det = dot(e1, pvec);
        if (std::fabs(det) < epsilon) return false;
        double inv_det = 1.0 / det; // for fractions

        // Ray is not parrelel. Does it intersect?

        // A point inside the triangle can be represented as P = A + u*e1 + v*e2 : u >= 0, v >= 0, u + v <= 1

        vec3 tvec = r.origin() - v[1].p;
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

        rec.record(r, t, normal, this);
        return true;

    }

    vec2 get_tcoords(const point& p) const override {

        float d = (v[1].p[1]-v[2].p[1])*(v[0].p[0]-v[2].p[0]) + (v[2].p[1]-v[1].p[1])*(v[0].p[1]-v[2].p[1]);
        d = 1 / d;


        float l0 = d * ((v[1].p[1]-v[2].p[1])*(p[0] - v[2].p[0])+(v[2].p[0]-v[1].p[0])*(p[1]-v[2].p[1]));
        float l1 = d * ((v[2].p[1]-v[0].p[1])*(p[0] - v[2].p[0])+(v[0].p[0]-v[2].p[0])*(p[1]-v[2].p[1]));
        float l2 = 1-l1-l2;

        return l0*v[0].t_coords+l1*v[1].t_coords+l2*v[2].t_coords;

    }

};