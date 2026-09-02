# Rendering Engine Conventions and Homogeneous Coordinate Math

This document captures the conventions used in the active codebase for geometry, camera math, and ray tracing. It is intended to reduce ambiguity when mixing points, vectors, and homogeneous coordinates.

## 1. Core convention: points vs vectors

The project uses a homogeneous 4D representation for spatial values:

- Point: `[w, x, y, z]` with `w = 1`
- Vector / direction: `[w, x, y, z]` with `w = 0`
- Spatial math uses only the `x, y, z` portion

The canonical definition lives in `src/utilities/linear_algebra/linear_algebra.hpp`.

The index mapping is:

- `P = 0`
- `X = 1`
- `Y = 2`
- `Z = 3`

Examples:

```cpp
point origin = make_point(0.0, 0.0, 0.0);
// [1, 0, 0, 0]

point up = make_vector(0.0, 1.0, 0.0);
// [0, 0, 1, 0]
```

Important rule:

- if it is a position in space, use `make_point(...)`
- if it is a direction, offset, basis vector, or normal, use `make_vector(...)`

## 2. Why this uses homogeneous coordinates

The renderer stores positions in projective space so the same code can express:

- points as affine positions
- vectors as direction/offset quantities
- ray origins and ray directions in a consistent 4D representation

This allows formulas like:

```cpp
point at(double t) const {
    return orig + t * dir;
}
```

to behave as expected when `orig` is a point and `dir` is a vector.

## 3. The project-wide rule

Every spatial value must be interpreted using this rule:

- `w = 1` means position
- `w = 0` means direction/vector
- `x, y, z` are the spatial coordinates

If code mixes those semantics, the renderer breaks in subtle ways:

- normals point the wrong way
- camera basis vectors become invalid
- ray intersections no longer match true geometry
- triangle and quad hit tests drift or fail entirely

## 4. Dot products and norms

The project intentionally ignores the homogeneous component when taking dot products and norms.

From `linear_algebra.hpp`:

```cpp
template <int N>
double dot(const vec<N>& v1, const vec<N>& v2) {
    double res = 0;
    int start = (N >= 4) ? 1 : 0;
    for (int i = start; i < N; i++) {
        res += v1[i] * v2[i];
    }
    return res;
}
```

This means for 4D values, only `X/Y/Z` are used in spatial calculations.

The same applies to length and normalization:

```cpp
template <int N>
double norm_squared(const vec<N>& v) {
    return dot(v, v);
}
```

This is why vectors must not accidentally be stored with `w = 1`.

## 5. Cross products

Cross products also operate on the spatial coordinates only.

The canonical 4D cross product implementation is:

```cpp
inline vec<4> cross(const vec<4>& v1, const vec<4>& v2) {
    vec<4> res;

    vec<4> a1 = homogenize(v1);
    vec<4> a2 = homogenize(v2);

    res[P] = 0;
    res[X] = a1[Y] * a2[Z] - a1[Z] * a2[Y];
    res[Y] = a1[Z] * a2[X] - a1[X] * a2[Z];
    res[Z] = a1[X] * a2[Y] - a1[Y] * a2[X];

    return res;
}
```

This creates a vector result with `w = 0` and spatial components equal to the usual 3D cross product.

## 6. Helper constructors and conversions

The code provides helpers for clarity and consistency:

```cpp
inline point make_point(double x, double y, double z) {
    point p;
    p[P] = 1;
    p[X] = x;
    p[Y] = y;
    p[Z] = z;
    return p;
}

inline point make_vector(double x, double y, double z) {
    point v;
    v[P] = 0;
    v[X] = x;
    v[Y] = y;
    v[Z] = z;
    return v;
}
```

These helpers are preferred over raw 4D assignments because they make the semantic distinction explicit.

## 7. Camera convention

The camera uses the same rules:

- `camera_center` is a point
- `facing` is a point
- `camera_up` is a vector
- basis vectors `b_u`, `b_v`, `b_w` are vectors

Example from `src/camera/cam_controls.hpp`:

```cpp
point camera_center = make_point(0, 0, 0);
point facing = make_point(0, 0, -1);
point camera_up = make_vector(0, 1, 0);

point b_u = make_vector(1, 0, 0);
point b_v = make_vector(1, 0, 0);
point b_w = make_vector(1, 0, 0);
```

The camera basis is calculated using vector arithmetic, not point arithmetic.

## 8. Ray convention

The ray type in `src/utilities/ray.hpp` is designed around the same semantics:

```cpp
class Ray {
    public:
        Ray(const point& origin, const point& direction, double time = 0)
            : orig(origin), dir(direction), tm(time) {}

        const point& origin() const { return orig; }
        const point& direction() const { return dir; }

        point at(double t) const {
            return orig + t * dir;
        }
};
```

Interpretation:

- `origin` is a point
- `direction` is a vector
- `at(t)` produces a point along the ray

## 9. Primitive convention

Primitives follow the same semantics:

- sphere center is a point
- triangle vertices are points
- quad anchor is a point
- edges between vertices are vectors
- normals are vectors

Examples:

```cpp
s.objects.add(std::make_shared<Sphere>(make_point(0, 2, 0), 2, material));

s.objects.add(std::make_shared<Quad>(
    make_point(3, 1, -2),
    make_vector(2, 0, 0),
    make_vector(0, 2, 0),
    material
));
```

This is the intended semantically correct layout.

## 10. Safety rules and common pitfalls

The main causes of rendering corruption in this codebase have been:

- treating a vector as a point by leaving `w = 1`
- treating a point as a vector by using position values in direction formulas
- mixing 3D and homogeneous math without homogenizing properly
- computing camera basis vectors from points instead of vectors
- using raw 4D constructors without explicit semantic intent

The recommended practice is:

- always build spatial values with `make_point(...)` or `make_vector(...)`
- never hand-write `[1, x, y, z]` or `[0, x, y, z]` without intention
- treat `w` as a semantic tag, not just another coordinate

## 11. Rule of thumb

Use the following mental model:

- a position is a point
- a displacement is a vector
- the homogeneous coordinate is what distinguishes them

If a value is a location in space: use `make_point(...)`.
If a value is a direction, offset, normal, or basis vector: use `make_vector(...)`.

This is the convention the active renderer expects.

## 12. Summary

The project’s accepted convention is:

- point = `[1, x, y, z]`
- vector = `[0, x, y, z]`
- spatial operations act on `x, y, z`
- `w` is semantic metadata, not a normal spatial axis

Following this convention consistently keeps camera math, ray math, normals, intersections, and scene setup coherent.
