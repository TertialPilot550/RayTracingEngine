#pragma once
#include "../../../main.hpp"


/**
 * @brief Default texture coordinates #0 for triangles
 */
static point2D vertex_0() {
    return point2D(0, 0);
}

/**
 * @brief Default texture coordinates #1 for triangles
 */
static point2D vertex_1() {
    return point2D(0, 1);
}

/**
 * @brief Default texture coordinates #2 for triangles
 */
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

    /**
     * @brief Ray Tracing Collision
     */
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

    /**
     * @brief Triangle texture mapping
     */
    point2D get_tcoords(const point& p, double time) const override {
        // Find the object at this time
        const point v0 = moved_point(0, time);
        const point v1 = moved_point(1, time);
        const point v2 = moved_point(2, time);

        // Barycentric Coordinates
        const double area = (v1[X] - v0[X]) * (v2[Y] - v0[Y]) - (v1[Y] - v0[Y]) * (v2[X] - v0[X]);
        if (std::fabs(area) < 1e-12) return point2D(0, 0);
        const double b0 = ((v1[X] - p[X]) * (v2[Y] - p[Y]) - (v1[Y] - p[Y]) * (v2[X] - p[X])) / area;
        const double b1 = ((v2[X] - p[X]) * (v0[Y] - p[Y]) - (v2[Y] - p[Y]) * (v0[X] - p[X])) / area;
        
        // Interpolate the texture based on barycentric coordinates
        return b0 * t_coords[0] + b1 * t_coords[1] + (1.0 - b0 - b1) * t_coords[2];
    }

    /**
     * @brief Rasterize a triangle to the screen
     */
    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& proj_matrix, double* depth_buff, rgb* color_buff, double time) const override {
        // Find the object at this moment in time
        point p[3];
        for (int i = 0; i < 3; ++i) {
            p[i] = homogenize(viewport_matrix * homogenize(proj_matrix * moved_point(i, time)));
        }

        const double area = (p[1][X] - p[0][X]) * (p[2][Y] - p[0][Y]) - (p[1][Y] - p[0][Y]) * (p[2][X] - p[0][X]);
        if (std::fabs(area) < 1e-12) return;

        const int min_x = std::max(0, static_cast<int>(std::floor(std::min({p[0][X], p[1][X], p[2][X]}))));
        const int max_x = std::min(screen_size[0] - 1, static_cast<int>(std::ceil(std::max({p[0][X], p[1][X], p[2][X]}))));
        const int min_y = std::max(0, static_cast<int>(std::floor(std::min({p[0][Y], p[1][Y], p[2][Y]}))));
        const int max_y = std::min(screen_size[1] - 1, static_cast<int>(std::ceil(std::max({p[0][Y], p[1][Y], p[2][Y]}))));

        // Edge function for barycentric coordinates
        auto edge = [](const point& p0, const point& p1, double x, double y) {
            return (x - p0[X]) * (p1[Y] - p0[Y]) -
                (y - p0[Y]) * (p1[X] - p0[X]);
        };

        // Returns a boolean if the point p0 is closer to the top left corner than p1 (edge overlap handling)
        auto closer_to_top_left = [](const point& p0, const point& p1) {
            return p0[Y] < p1[Y] || (p0[Y] == p1[Y] && p0[X] > p1[X]);
        };

        // For each pixel in the bounded area...
        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {

                // Calculate Barycentric coordinates for that pixel
                const double px = x + 0.5;
                const double py = y + 0.5;
                double w0 = edge(p[1], p[2], px, py) / area;
                double w1 = edge(p[2], p[0], px, py) / area;
                double w2 = edge(p[0], p[1], px, py) / area;
                bool inside = true;

                // Determine if the pixel is inside of the triangle
                if (area < 0) {
                    w0 = -w0;
                    w1 = -w1;
                    w2 = -w2;
                    inside = (w0 > 0 || (std::fabs(w0) <= 1e-10 && closer_to_top_left(p[2], p[1]))) && (w1 > 0 || (std::fabs(w1) <= 1e-10 && closer_to_top_left(p[0], p[2]))) && (w2 > 0 || (std::fabs(w2) <= 1e-10 && closer_to_top_left(p[1], p[0])));
                } else {
                    inside = (w0 > 0 || (std::fabs(w0) <= 1e-10 && closer_to_top_left(p[1], p[2]))) && (w1 > 0 || (std::fabs(w1) <= 1e-10 && closer_to_top_left(p[2], p[0]))) && (w2 > 0 || (std::fabs(w2) <= 1e-10 && closer_to_top_left(p[0], p[1])));
                }
                if (!inside) continue;

                const double depth = w0 * p[0][Z] + w1 * p[1][Z] + w2 * p[2][Z];
                const int index = x + y * screen_size[0];
                // Conditional depth buffer update
                if (depth < depth_buff[index]) {
                    depth_buff[index] = depth;

                    // Shader definition
                    auto shade = [this, &p](double w0, double w1, double w2, double) {
                        const point2D uv = w0 * t_coords[0] + w1 * t_coords[1] + w2 * t_coords[2];
                        const point position = w0 * p[0] + w1 * p[1] + w2 * p[2];
                        return surf && surf->mat ? surf->mat->at(uv[0], uv[1], position) : color();
                    };

                    // Find color
                    color_buff[index] = color_correction_to_rgb(shade(w0, w1, w2, w0 * p[0][P] + w1 * p[1][P] + w2 * p[2][P]));
                }
            }
        }
    }
};


