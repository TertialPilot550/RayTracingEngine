#pragma once
#include "../../main.hpp"

vec<3> barycentric_coords(point v[3], int x, int y) {
    float d = (v[1][1]-v[2][1])*(v[0][0]-v[2][0]) + (v[2][1]-v[1][1])*(v[0][1]-v[2][1]);
    d = 1 / d;

    float l0 = d * ((v[1][1]-v[2][1])*(x - v[2][0])+(v[2][0]-v[1][0])*(y-v[2][1]));
    float l1 = d * ((v[2][1]-v[0][1])*(x - v[2][0])+(v[0][0]-v[2][0])*(y-v[2][1]));
    float l2 = 1-l0-l1;

    return vec<3>(l0, l1, l2);
}

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
        point vpoints[] = {v[0].p, v[1].p, v[2].p, v[3].p};
        vec<3> b_coords = barycentric_coords(vpoints, p[1], p[2]);
        return b_coords[0]*v[0].t_coords+b_coords[1]*v[1].t_coords+b_coords[2]*v[2].t_coords;
    }

    void rasterize(int screen_size[2], Matrix<4,4>& viewport_matrix, Matrix<4,4>& proj_matrix, double* depth_buff, color* color_buff) {
        if (is_at_infinity(v[0].p) || is_at_infinity(v[1].p) || is_at_infinity(v[2].p)) return;

        // Affine
        point p0 = proj_matrix * v[0].p;
        point p1 = proj_matrix * v[1].p;
        point p2 = proj_matrix * v[2].p;

        // Projective Division
        p0 = homogenize(p0);
        p1 = homogenize(p1);
        p2 = homogenize(p2);

        // Affine
        p0 = viewport_matrix * p0;
        p1 = viewport_matrix * p1;
        p2 = viewport_matrix * p2;
        point t_points[] = {p0, p1, p2};

        // Find a bounding box
        int min_x = std::min(std::min(p0[1], p1[1]), p2[1]);
        int max_x = std::max(std::max(p0[1], p1[1]), p2[1]);
        int min_y = std::min(std::min(p0[2], p1[2]), p2[2]);
        int max_y = std::max(std::max(p0[2], p1[2]), p2[2]);

        // For each pixel within the projection bounds...
        for (int i = min_x; i <= max_x; i++) {
            for (int j = min_y; j <= max_y; j++) {
        
                // Determine if the pixel is part of the triangle 
                vec<3> b_coords = barycentric_coords(t_points, i, j);
                Interval unit = Interval(0,1);
                if (unit.contains(b_coords[0]) && !unit.contains(b_coords[1]) && unit.contains(b_coords[2])) {
                    // point is in the traingle
                    // Use barycentric coordinates to interpolate both depth and texture coordinates
                    int z = b_coords[0] * p0[3] + b_coords[1] * p1[3] + b_coords[2] * p2[3];
                    point2D t = b_coords[0]*v[0].t_coords+b_coords[1]*v[1].t_coords+b_coords[2]*v[2].t_coords;

                    point pp = b_coords[0]*p0+b_coords[1]*p1+b_coords[2]*p2;
                    color c = surf->mat->at(t[0], t[1], pp); 

                    if (z < depth_buff[i + j * screen_size[0]]) {
                        depth_buff[i + j * screen_size[0]] = z;
                        color_buff[i + j * screen_size[0]] = c;
                    }
                }

            }
        }
    }
};

