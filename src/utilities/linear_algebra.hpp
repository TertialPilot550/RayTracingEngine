#pragma once

#include <cmath>
#include <initializer_list>
#include <iostream>
#include <array>
#include <stdexcept>
#include <type_traits>
#include "util.hpp"

// Helper Indicies

// Projective 3D Space
#define P 0
#define X 1
#define Y 2
#define Z 3

// RGB
#define RED 0 
#define GREEN 1
#define BLUE 2



/**
 * @brief Arbitrary N-vector of doubles
 */
template <typename... Ts>
struct are_arithmetic;

template <>
struct are_arithmetic<> : std::true_type {};

template <typename T, typename... Ts>
struct are_arithmetic<T, Ts...> : std::integral_constant<bool, std::is_arithmetic<T>::value && are_arithmetic<Ts...>::value> {};

template <int N>
class vec {

    public:

    vec() : elem{} {}
    vec(const vec&) = default;

    template <typename T, typename U, typename V, typename std::enable_if<N == 4 && std::is_arithmetic<T>::value && std::is_arithmetic<U>::value && std::is_arithmetic<V>::value, int>::type = 0>
    vec(T x, U y, V z) : elem{} {
        elem[0] = 1.0;
        elem[1] = static_cast<double>(x);
        elem[2] = static_cast<double>(y);
        elem[3] = static_cast<double>(z);
    }

    // Variadic constructor for initialization
    template <typename... Args,
              typename std::enable_if<are_arithmetic<Args...>::value, int>::type = 0>
    vec(Args... args) : elem{} {
        static_assert(sizeof...(args) == N, "Number of arguments must match vector dimension");
        init_from_args(0, args...);
    }

    double& operator[](int ind) { 
        if (ind < 0 || ind >= N)
        throw std::out_of_range("vec index out of range");
        return elem[ind]; 
    }

    double operator[](int ind) const {
        if (ind < 0 || ind >= N)
        throw std::out_of_range("vec index out of range");
        return elem[ind];
    }

    vec<N>& operator=(const vec<N>& v) = default;

    vec<N>& operator=(double d) {
        for (int i = 0; i < N; i++) {
            elem[i] = d;
        }
        return *this;
    }

    static vec<N> random() {
        vec<N> result;
        for (int i = 0; i < N; ++i) {
            result[i] = random_double();
        }
        return result;
    }

    static vec<N> random(double min, double max) {
        vec<N> result;
        for (int i = 0; i < N; ++i) {
            result[i] = random_double(min, max);
        }
        return result;
    }

    size_t size() {return elem.size();}
    
    private:
    
    // Base case: single argument
    template <typename T>
    void init_from_args(int idx, T value) {
        elem[idx] = static_cast<double>(value);
    }

    // Recursive case: multiple arguments
    template <typename T, typename... Args>
    void init_from_args(int idx, T value, Args... args) {
        elem[idx] = static_cast<double>(value);
        init_from_args(idx + 1, args...);
    }
    
    public:
    
    std::array<double, N> elem;

};

template <int N>
vec<N> homogenize(const vec<N>& v);

using color = vec<3>;
using point = vec<4>;
using point2D = vec<2>;

/**
 * @brief Arbitrary NxM matrix of doubles
 */
template <int N, int M>
class Matrix {
    public:

    Matrix() : elem{} {}
    Matrix(const Matrix&) = default;

    std::array<double, M>& operator[](int ind) {
        if (ind < 0 || ind >= N)
        throw std::out_of_range("vec index out of range");
        return elem[ind];
    }

    std::array<double, M> operator[](int ind) const {
        if (ind < 0 || ind >= N)
        throw std::out_of_range("vec index out of range");
        return elem[ind];
    }


    Matrix<N,M>& operator=(const Matrix<N,M>& m) = default;

    Matrix<N,M>& operator=(double d) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                elem[i][j] = d;
            }
        }
        return *this;
    }

    size_t size_rows() {return N;}
    size_t size_cols() {return M;}
    size_t size_elem() {return N * M;}

    std::array<std::array<double, M>, N> elem;

};

/*
 * Linear Vector Operations 
 */

template <int N>
vec<N>& operator+=(vec<N>& v1, const vec<N>& v2) {
    for (int i = 0; i < N; i++) {
        v1.elem[i] += v2.elem[i];
    }
    return v1;
}

template <int N>
vec<N>& operator-=(vec<N>& v1, const vec<N>& v2) {
    for (int i = 0; i < N; i++) {
        v1.elem[i] -= v2.elem[i];
    }
    return v1;
}

template <int N>
vec<N>& operator*=(vec<N>& v1, const double d) {
    for (int i = 0; i < N; i++) {
        v1.elem[i] *= d;
    }
    return v1;
}

template <int N>
vec<N>& operator*=(const double d, vec<N>& v) {
    for (int i = 0; i < N; i++) {
        v.elem[i] *= d;
    }
    return v;
}

template <int N>
vec<N>& operator/=(vec<N>& v1, const double d) {
    for (int i = 0; i < N; i++) {
        v1.elem[i] /= d;
    }
    return v1;
}

template <int N>
vec<N> operator-(const vec<N>& v) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res.elem[i] = -v.elem[i];
    }
    return res;
}

template <int N>
vec<N> operator+(const vec<N>& v1, const vec<N>& v2) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res[i] = v1[i] + v2[i];
    }
    return res;
}

template <int N>
vec<N> operator-(const vec<N>& v1, const vec<N>& v2) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res[i] = v1[i] - v2[i];
    }
    return res;
}


template <int N>
vec<N> operator*(const double d, const vec<N>& v) {
    vec<N> res = v;
    res *= d;
    return res;
}

template <int N>
vec<N> operator*(const vec<N>& v, const double d) {
    return d * v;
}

template <int N>
vec<N> operator*(const vec<N>& v1, const vec<N>& v2) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res[i] = v1[i] * v2[i];
    }
    return res;
}

template <int N>
vec<N> operator/(const vec<N>& v, const double d) {
    return (1/d) * v;
}


/*
 * Vector Products
 */

template <int N>
double dot(const vec<N>& v1, const vec<N>& v2) {
    double res = 0;
    int start = (N >= 4) ? 1 : 0;
    for (int i = start; i < N; i++) {
        res += v1[i] * v2[i];
    }
    return res;
}

template <int N>
double norm_squared(const vec<N>& v) {
    return dot(v, v);
}

template <int N>
double norm(const vec<N>& v) {
    return std::sqrt(norm_squared(v));
}

/**
 * @brief 3D Cross Product
 */
inline vec<3> cross(const vec<3>& v1, const vec<3>& v2) {
    vec<3> res;
    
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];

    return res;
}

/**
 * @brief 3D Cross Product Using Homogeneous Coordinated
 */
inline vec<4> cross(const vec<4>& v1, const vec<4>& v2) {
    vec<4> res;

    vec<4> a1 = homogenize(v1);
    vec<4> a2 = homogenize(v2);

    // Spatial coordinates live in X/Y/Z; the homogeneous coordinate is always w.
    // For a 4D point/vector layout [w, x, y, z], the 3D cross product is:
    // (x,y,z) x (x2,y2,z2) = (y*z2 - z*y2, z*x2 - x*z2, x*y2 - y*x2)
    res[P] = 0;
    res[X] = a1[Y] * a2[Z] - a1[Z] * a2[Y];
    res[Y] = a1[Z] * a2[X] - a1[X] * a2[Z];
    res[Z] = a1[X] * a2[Y] - a1[Y] * a2[X];

    return res;
}


/*
 * Vector Utilities 
 */

 template <int N>
vec<N> unit_vector(const vec<N>& v) {
    return v / norm(v);
}

template <int N>
vec<N> reflect(const vec<N>& v, const vec<N>& n) {
    return v - 2*dot(v,n)*n;
}

template <int N>
vec<N> refract(const vec<N>& uv, const vec<N>& n, double etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec<N> r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec<N> r_out_parallel = -std::sqrt(std::fabs(1.0 - norm_squared(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

template <int N>
bool near_zero(const vec<N>& v) {
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    for (int i = 0; i < N; i++) {
        if (std::fabs(v.elem[i]) >= s) return false;
    }
    return true;
}

template <int N>
vec<N> random() {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res.elem[i] = random_double();
    }
    return res;
}

template <int N>
vec<N> random(double min, double max) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        res.elem[i] = random_double(min, max);
    }
    return res;
}

template <int N>
vec<N> random_vec() {
    return random<N>();
}

template <int N>
vec<N> random_vec(double min, double max) {
    return random<N>(min, max);
}

template <int N>
vec<N> random_unit_vector() {
    while (true) {
        vec<N> p = random_vec<N>(-1, 1);
        if (N >= 4) p[P] = 0;
        auto lensq = norm_squared(p);
        if (1e-160 < lensq && lensq <= 1)
            return p / norm(p);
    }
}

template <int N>
vec<N> random_on_hemisphere(const vec<N>& normal) {
    vec<N> on_unit_sphere = random_unit_vector<N>();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}
template <int N>
vec<N> random_in_unit_disk() {
    while (true) {
        vec<N> p = vec<N>();
        p[P] = 0;
        p[X] = random_double(-1,1);
        p[Y] = random_double(-1,1);
        if (N > Z) p[Z] = 0;
        if (norm_squared(p) < 1)
            return p;
    }
}

template <int N>
vec<N> as_vector(const vec<N>& v) {
    vec<N> res = v;
    if (N >= 4) res[P] = 0;
    return res;
}

template <int N>
vec<N> as_point(const vec<N>& v) {
    vec<N> res = v;
    if (N >= 4) res[P] = 1;
    return res;
}

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

template <int N>
bool is_at_infinity(const vec<N>& v) {
    return v[P] == 0;
}


template <int N>
vec<N> homogenize(const vec<N>& v) {
    if (is_at_infinity(v)) {
        return v;
    } else {
        vec<N> res = v;
        for (int i = 1; i < N; i++) {
            res[i] /= res[P];
        }
        return res;
    }
}


/**
 * @brief Display a vector to standard output
 */
template <int N>
std::ostream& operator<<(std::ostream& out, const vec<N>& v) {
    out << "[";
    if (v.elem.size() > 0) out << v.elem[0];
    for (int i = 1; i < N; i++) {
        out << ", " << v.elem[i];
    }
    out << "]";
    return out;
}

/*
 * Matrix Operations
 */

/**
 * @brief Matrix Multiplication
 */
template <int N, int M, int K>
Matrix<N,K> operator*(Matrix<N,M>& m1, Matrix<M,K>& m2) {
    Matrix<N,K> res;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            for (int k = 0; k < M; k++) {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return res;
}

template <int N, int M>
vec<M> operator*(vec<N> vec, Matrix<N,M>) {
    vec<M> res;
    for (int i = 0; i < M; i++) {
        for (int k = 0; k < N; k++) {
            res[i] += m1[k] * m2[k][i];
        }
    }
    return res;
}

template <int N, int M>
vec<N> operator*(Matrix<N,M>, vec<M> vec) {
    vec<N> res;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < M; k++) {
            res[i] += m1[i][k] * m2[k];
        }
    }
}


// Addition/Inversion
template <int N, int M>
Matrix<N,M> operator+(const Matrix<N,M>& m1, const Matrix<N,M>& m2) {
    Matrix<N,M> res = m1;
    res += m2;
    return res;
}
template <int N, int M>
Matrix<N,M> operator-(const Matrix<N,M>& m1, const Matrix<N,M>& m2) {
    Matrix<N,M> res = m1;
    res -= m2;
    return res;
}
template <int N, int M>
Matrix<N,M> operator-(const Matrix<N,M>& m1) {
    Matrix<N,M> res;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            res[i][j] = -m1[i][j];
        }
    }
    return res;
}

// Scalar multiplication
template <int N, int M>
Matrix<N,M> operator*(const Matrix<N,M>& m1, double d) {
    Matrix<N,M> res = m1;
    res *= d;
    return res;
}
template <int N, int M>
Matrix<N,M> operator*(double d, const Matrix<N,M>& m1) {
    Matrix<N,M> res = m1;
    res *= d;
    return res;
}
template <int N, int M>
Matrix<N,M> operator/(const Matrix<N,M>& m1, double d) {
    Matrix<N,M> res = m1;
    res /= d;
    return res;
}

// Addition/Inversion
template <int N, int M>
Matrix<N,M>& operator+=(Matrix<N,M>& m1, const Matrix<N,M>& m2) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m1[i][j] += m2[i][j];
        }
    }
    return m1;
}
template <int N, int M>
Matrix<N,M>& operator-=(Matrix<N,M>& m1, const Matrix<N,M>& m2) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m1[i][j] -= m2[i][j];
        }
    }
    return m1;
}


// Scalar multiplication
template <int N, int M>
Matrix<N,M>& operator*=(Matrix<N,M>& m, double d) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m[i][j] *= d;
        }
    }
    return m;
}
template <int N, int M>
Matrix<N,M>& operator*=(double d, Matrix<N,M>& m) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m[i][j] *= d;
        }
    }
    return m;
}
template <int N, int M>
Matrix<N,M>& operator/=(Matrix<N,M>& m, double d) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            m[i][j] /= d;
        }
    }
    return m;
}

double determinant(Matrix<2,2> mat) {
    return mat[0][0]*mat[1][1]-mat[0][1]*mat[1][0];
}

template <int N, int M>
std::ostream& operator<<(std::ostream& out, const Matrix<N,M>& m) {

    return out;
}

int m_ind(int w, int i, int j) {
    return i * w + j;
}

Matrix<4,4> world_to_camera_matrix(const point& u, const point& v, const point& w, point cam) {
    Matrix<4,4> res;
    
    for (int i = 0; i < 3; i++) {
        res[0][i] = u[i]; // set the top row
        res[2][i] = 0; // set bottom row at the same time
    }

    for (int i = 0; i < 3; i++) {
        res[1][i] = v[i]; 
    }

    for (int i = 0; i < 3; i++) {
        res[2][i] = w[i]; 
    }

    res[3][0] = dot<4>(-u, cam);
    res[3][1] = dot<4>(-v, cam);
    res[3][2] = dot<4>(-w, cam);
    res[3][3] = 1;

    return res;
}

Matrix<4,4> projection_matrix(float n, float f, float t, float b, float l, float r) {
    Matrix<4,4> res;

    res[0][0] = (2*n) / (r-l);
    res[1][1] = (2*n) / (t-b);
    res[2][2] = -((f+n)/(f-n));
    res[2][3] = -((2*f*n)/(f-n));
    res[2][3] = -1;
    res[0][2] = (r+l) / (r-l);
    res[1][2] = (t+b) / (t-b);

    return res;
}

Matrix<4,4> viewport_matrix(int w, int h) {
    Matrix<4,4> res;

    res[0][0] = w/2;
    res[0][3] = w/2;
    res[1][1] = -h/2;
    res[1][3] = h/2;
    res[2][2] = 0.5;
    res[2][3] = 0.5;
    res[3][3] = 1;

    return res;
}







