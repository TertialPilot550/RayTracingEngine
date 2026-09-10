#pragma once
#include "../../../main.hpp"

static point2D vertex_0() {
        return point2D(0, 0);
    }

static point2D vertex_1() {
    return point2D(0, 1);
}

static point2D vertex_2() {
    return point2D(1, 0.5);
}

/**
 * @brief Triangle Primitive
 */
class Triangle : public GeometricPrimitive<3> {

    public:
    point2D t_coords[3];

    Triangle(point p1, point p2, point p3, std::shared_ptr<Surface> surface, point2D t1 = vertex_0(), point2D t2 = vertex_1(), point2D t3 = vertex_2()) : GeometricPrimitive<3>(surface, p1, p2, p3) {

        t_coords[0] = t1;
        t_coords[1] = t2;
        t_coords[2] = t3;
    } 

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {

        // If any vertex is at infinity, there is not intersection
        if (is_at_infinity(skeleton[0]) || is_at_infinity(skeleton[1]) || is_at_infinity(skeleton[2])) return false;
        point p0 = homogenize(moved_point(0, r.time()));
        point p1 = homogenize(moved_point(1, r.time()));
        point p2 = homogenize(moved_point(2, r.time()));

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
        const point v0 = skeleton[0];
        const point v1 = skeleton[1];
        const point v2 = skeleton[2];
        const double area = (v1[X] - v0[X]) * (v2[Y] - v0[Y]) -
                            (v1[Y] - v0[Y]) * (v2[X] - v0[X]);
        if (std::fabs(area) < 1e-12) return point2D(0, 0);
        const double b0 = ((v1[X] - p[X]) * (v2[Y] - p[Y]) -
                           (v1[Y] - p[Y]) * (v2[X] - p[X])) / area;
        const double b1 = ((v2[X] - p[X]) * (v0[Y] - p[Y]) -
                           (v2[Y] - p[Y]) * (v0[X] - p[X])) / area;
        return b0 * t_coords[0] + b1 * t_coords[1] +
               (1.0 - b0 - b1) * t_coords[2];
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix,
                   mat<4,4>& proj_matrix, double* depth_buff,
                   rgb* color_buff) const override {
        point p[3];
        for (int i = 0; i < 3; ++i) {
            p[i] = homogenize(viewport_matrix * homogenize(
                proj_matrix * moved_point(i, 0.5)));
        }
        rasterize_triangle({p[0]}, {p[1]}, {p[2]}, screen_size[0],
                           screen_size[1], depth_buff, color_buff,
                           [this, &p](double w0, double w1, double w2,
                                      double) {
            const point2D uv = w0 * t_coords[0] + w1 * t_coords[1] +
                               w2 * t_coords[2];
            const point position = w0 * p[0] + w1 * p[1] + w2 * p[2];
            return surf && surf->mat ? surf->mat->at(uv[0], uv[1], position)
                                     : color();
        });
    }
};
