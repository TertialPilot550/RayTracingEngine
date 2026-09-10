#include <cassert>
#include <cmath>
#include <iostream>

#include "../src/utilities/linear_algebra.hpp"

template <int N, int M>
using Matrix = mat<N, M>;


// ============================================================
// Helpers
// ============================================================

const double EPS = 1e-9;

bool approx(double a, double b, double eps = EPS) {
    return std::abs(a - b) < eps;
}

template <int N>
bool approx_vec(const vec<N>& a, const vec<N>& b, double eps = EPS) {
    for (int i = 0; i < N; ++i) {
        if (!approx(a.elem[i], b.elem[i], eps))
            return false;
    }

    return true;
}

template <int N, int M>
bool approx_matrix(const Matrix<N, M>& a,
                  const Matrix<N, M>& b,
                  double eps = EPS) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (!approx(a.elem[i][j], b.elem[i][j], eps))
                return false;
        }
    }

    return true;
}


// ============================================================
// Vector tests
// ============================================================

void test_vector_indexing() {
    std::cout << "Testing vector indexing...\n";

    vec<3> v;

    v[0] = 1.0;
    v[1] = 2.0;
    v[2] = 3.0;

    assert(v[0] == 1.0);
    assert(v[1] == 2.0);
    assert(v[2] == 3.0);

    v[0] = 10.0;
    v[1] = 20.0;
    v[2] = 30.0;

    assert(v[0] == 10.0);
    assert(v[1] == 20.0);
    assert(v[2] == 30.0);

    std::cout << "  PASSED\n";
}


void test_vector_size() {
    std::cout << "Testing vector size...\n";

    vec<5> v;

    assert(v.size() == 5);

    std::cout << "  PASSED\n";
}


void test_vector_assignment() {
    std::cout << "Testing vector assignment...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    vec<3> b;

    b = a;

    assert(b[0] == 1.0);
    assert(b[1] == 2.0);
    assert(b[2] == 3.0);

    std::cout << "  PASSED\n";
}


// ============================================================
// Vector arithmetic
// ============================================================

void test_vector_addition() {
    std::cout << "Testing vector addition...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    vec<3> b;
    b[0] = 4.0;
    b[1] = 5.0;
    b[2] = 6.0;

    vec<3> result = a + b;

    assert(result[0] == 5.0);
    assert(result[1] == 7.0);
    assert(result[2] == 9.0);

    std::cout << "  PASSED\n";
}


void test_vector_addition_assignment() {
    std::cout << "Testing vector +=...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    vec<3> b;
    b[0] = 4.0;
    b[1] = 5.0;
    b[2] = 6.0;

    a += b;

    assert(a[0] == 5.0);
    assert(a[1] == 7.0);
    assert(a[2] == 9.0);

    std::cout << "  PASSED\n";
}


void test_vector_subtraction() {
    std::cout << "Testing vector subtraction...\n";

    vec<3> a;
    a[0] = 5.0;
    a[1] = 7.0;
    a[2] = 9.0;

    vec<3> b;
    b[0] = 1.0;
    b[1] = 2.0;
    b[2] = 3.0;

    vec<3> result = a - b;

    assert(result[0] == 4.0);
    assert(result[1] == 5.0);
    assert(result[2] == 6.0);

    std::cout << "  PASSED\n";
}


void test_vector_subtraction_assignment() {
    std::cout << "Testing vector -=...\n";

    vec<3> a;
    a[0] = 5.0;
    a[1] = 7.0;
    a[2] = 9.0;

    vec<3> b;
    b[0] = 1.0;
    b[1] = 2.0;
    b[2] = 3.0;

    a -= b;

    assert(a[0] == 4.0);
    assert(a[1] == 5.0);
    assert(a[2] == 6.0);

    std::cout << "  PASSED\n";
}


void test_vector_negation() {
    std::cout << "Testing vector negation...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = -2.0;
    a[2] = 3.0;

    vec<3> result = -a;

    assert(result[0] == -1.0);
    assert(result[1] == 2.0);
    assert(result[2] == -3.0);

    std::cout << "  PASSED\n";
}


// ============================================================
// Scalar multiplication/division
// ============================================================

void test_vector_scalar_multiplication() {
    std::cout << "Testing vector scalar multiplication...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    vec<3> result1 = 2.0 * a;
    vec<3> result2 = a * 2.0;

    assert(result1[0] == 2.0);
    assert(result1[1] == 4.0);
    assert(result1[2] == 6.0);

    assert(result2[0] == 2.0);
    assert(result2[1] == 4.0);
    assert(result2[2] == 6.0);

    std::cout << "  PASSED\n";
}


void test_vector_scalar_multiplication_assignment() {
    std::cout << "Testing vector *=...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    a *= 3.0;

    assert(a[0] == 3.0);
    assert(a[1] == 6.0);
    assert(a[2] == 9.0);

    std::cout << "  PASSED\n";
}


void test_vector_scalar_division() {
    std::cout << "Testing vector division...\n";

    vec<3> a;
    a[0] = 2.0;
    a[1] = 4.0;
    a[2] = 6.0;

    vec<3> result = a / 2.0;

    assert(result[0] == 1.0);
    assert(result[1] == 2.0);
    assert(result[2] == 3.0);

    a /= 2.0;

    assert(a[0] == 1.0);
    assert(a[1] == 2.0);
    assert(a[2] == 3.0);

    std::cout << "  PASSED\n";
}


// ============================================================
// Vector products
// ============================================================

void test_dot_product() {
    std::cout << "Testing dot product...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;

    vec<3> b;
    b[0] = 4.0;
    b[1] = 5.0;
    b[2] = 6.0;

    double result = dot(a, b);

    assert(approx(result, 32));

    std::cout << "  PASSED\n";
}


void test_norm_squared() {
    std::cout << "Testing norm_squared...\n";

    vec<3> v;
    v[0] = 1.0;
    v[1] = 2.0;
    v[2] = 3.0;

    assert(approx(norm_squared(v), 14.0));

    std::cout << "  PASSED\n";
}


void test_norm() {
    std::cout << "Testing norm...\n";

    vec<3> v;
    v[0] = 3.0;
    v[1] = 4.0;
    v[2] = 0.0;

    assert(approx(norm(v), 5.0));

    std::cout << "  PASSED\n";
}


void test_cross_product() {
    std::cout << "Testing cross product...\n";

    vec<3> a;
    a[0] = 1.0;
    a[1] = 0.0;
    a[2] = 0.0;

    vec<3> b;
    b[0] = 0.0;
    b[1] = 1.0;
    b[2] = 0.0;

    vec<3> result = cross(a, b);

    assert(approx(result[0], 0.0));
    assert(approx(result[1], 0.0));
    assert(approx(result[2], 1.0));

    // Test anti-commutativity
    result = cross(b, a);

    assert(approx(result[0], 0.0));
    assert(approx(result[1], 0.0));
    assert(approx(result[2], -1.0));

    std::cout << "  PASSED\n";
}


// ============================================================
// Vector utilities
// ============================================================

void test_unit_vector() {
    std::cout << "Testing unit_vector...\n";

    vec<3> v;
    v[0] = 3.0;
    v[1] = 4.0;
    v[2] = 0.0;

    vec<3> result = unit_vector(v);

    assert(approx(result[0], 0.6));
    assert(approx(result[1], 0.8));
    assert(approx(result[2], 0.0));

    assert(approx(norm(result), 1.0));

    std::cout << "  PASSED\n";
}


void test_reflect() {
    std::cout << "Testing reflect...\n";

    vec<3> v;
    v[0] = 1.0;
    v[1] = -1.0;
    v[2] = 0.0;

    vec<3> n;
    n[0] = 0.0;
    n[1] = 1.0;
    n[2] = 0.0;

    vec<3> result = reflect(v, n);

    assert(approx(result[0], 1.0));
    assert(approx(result[1], 1.0));
    assert(approx(result[2], 0.0));

    std::cout << "  PASSED\n";
}


void test_near_zero() {
    std::cout << "Testing near_zero...\n";

    vec<3> tiny;
    tiny[0] = 1e-10;
    tiny[1] = -1e-10;
    tiny[2] = 1e-12;

    vec<3> normal;
    normal[0] = 1.0;
    normal[1] = 1e-10;
    normal[2] = 0.0;

    assert(near_zero(tiny));
    assert(!near_zero(normal));

    std::cout << "  PASSED\n";
}


// ============================================================
// Matrix tests
// ============================================================

void test_matrix_size() {
    std::cout << "Testing matrix sizes...\n";

    Matrix<2, 3> m;

    assert(m.size_rows() == 2);
    assert(m.size_cols() == 3);
    assert(m.size_elem() == 6);

    std::cout << "  PASSED\n";
}


void test_matrix_indexing() {
    std::cout << "Testing matrix indexing...\n";

    Matrix<2, 3> m;

    m[0][0] = 1.0;
    m[0][1] = 2.0;
    m[0][2] = 3.0;

    m[1][0] = 4.0;
    m[1][1] = 5.0;
    m[1][2] = 6.0;

    assert(m[0][0] == 1.0);
    assert(m[0][1] == 2.0);
    assert(m[0][2] == 3.0);

    assert(m[1][0] == 4.0);
    assert(m[1][1] == 5.0);
    assert(m[1][2] == 6.0);

    m[0][0] = 10.0;

    assert(m[0][0] == 10.0);

    std::cout << "  PASSED\n";
}


void test_matrix_assignment() {
    std::cout << "Testing matrix assignment...\n";

    Matrix<2, 2> a;

    a[0][0] = 1.0;
    a[0][1] = 2.0;
    a[1][0] = 3.0;
    a[1][1] = 4.0;

    Matrix<2, 2> b;

    b = a;

    assert(approx_matrix(a, b));

    std::cout << "  PASSED\n";
}


void test_matrix_addition() {
    std::cout << "Testing matrix addition...\n";

    Matrix<2, 2> a;
    a[0][0] = 1.0;
    a[0][1] = 2.0;
    a[1][0] = 3.0;
    a[1][1] = 4.0;

    Matrix<2, 2> b;
    b[0][0] = 5.0;
    b[0][1] = 6.0;
    b[1][0] = 7.0;
    b[1][1] = 8.0;

    Matrix<2, 2> result = a + b;

    assert(result[0][0] == 6.0);
    assert(result[0][1] == 8.0);
    assert(result[1][0] == 10.0);
    assert(result[1][1] == 12.0);

    std::cout << "  PASSED\n";
}


void test_matrix_subtraction() {
    std::cout << "Testing matrix subtraction...\n";

    Matrix<2, 2> a;
    a[0][0] = 5.0;
    a[0][1] = 6.0;
    a[1][0] = 7.0;
    a[1][1] = 8.0;

    Matrix<2, 2> b;
    b[0][0] = 1.0;
    b[0][1] = 2.0;
    b[1][0] = 3.0;
    b[1][1] = 4.0;

    Matrix<2, 2> result = a - b;

    assert(result[0][0] == 4.0);
    assert(result[0][1] == 4.0);
    assert(result[1][0] == 4.0);
    assert(result[1][1] == 4.0);

    std::cout << "  PASSED\n";
}


void test_matrix_negation() {
    std::cout << "Testing matrix negation...\n";

    Matrix<2, 2> a;

    a[0][0] = 1.0;
    a[0][1] = -2.0;
    a[1][0] = 3.0;
    a[1][1] = -4.0;

    Matrix<2, 2> result = -a;

    assert(result[0][0] == -1.0);
    assert(result[0][1] == 2.0);
    assert(result[1][0] == -3.0);
    assert(result[1][1] == 4.0);

    std::cout << "  PASSED\n";
}


void test_matrix_scalar_multiplication() {
    std::cout << "Testing matrix scalar multiplication...\n";

    Matrix<2, 2> a;

    a[0][0] = 1.0;
    a[0][1] = 2.0;
    a[1][0] = 3.0;
    a[1][1] = 4.0;

    Matrix<2, 2> result1 = a * 2.0;
    Matrix<2, 2> result2 = 2.0 * a;

    assert(result1[0][0] == 2.0);
    assert(result1[0][1] == 4.0);
    assert(result1[1][0] == 6.0);
    assert(result1[1][1] == 8.0);

    assert(approx_matrix(result1, result2));

    std::cout << "  PASSED\n";
}


void test_matrix_scalar_division() {
    std::cout << "Testing matrix scalar division...\n";

    Matrix<2, 2> a;

    a[0][0] = 2.0;
    a[0][1] = 4.0;
    a[1][0] = 6.0;
    a[1][1] = 8.0;

    Matrix<2, 2> result = a / 2.0;

    assert(result[0][0] == 1.0);
    assert(result[0][1] == 2.0);
    assert(result[1][0] == 3.0);
    assert(result[1][1] == 4.0);

    std::cout << "  PASSED\n";
}


// ============================================================
// Matrix multiplication
// ============================================================

void test_matrix_multiplication() {
    std::cout << "Testing matrix multiplication...\n";

    Matrix<2, 3> a;

    a[0][0] = 1.0;
    a[0][1] = 2.0;
    a[0][2] = 3.0;

    a[1][0] = 4.0;
    a[1][1] = 5.0;
    a[1][2] = 6.0;


    Matrix<3, 2> b;

    b[0][0] = 7.0;
    b[0][1] = 8.0;

    b[1][0] = 9.0;
    b[1][1] = 10.0;

    b[2][0] = 11.0;
    b[2][1] = 12.0;


    Matrix<2, 2> result = a * b;

    // [1 2 3] [7  8 ]   [58  64 ]
    // [4 5 6] [9  10] = [139 154]
    //         [11 12]

    assert(result[0][0] == 58.0);
    assert(result[0][1] == 64.0);
    assert(result[1][0] == 139.0);
    assert(result[1][1] == 154.0);

    std::cout << "  PASSED\n";
}


// ============================================================
// Random tests
// ============================================================

void test_random_range() {
    std::cout << "Testing random(min, max)...\n";

    const double MIN = -5.0;
    const double MAX = 10.0;

    for (int i = 0; i < 1000; ++i) {
        vec<3> v = random<3>(MIN, MAX);

        for (int j = 0; j < 3; ++j) {
            assert(v[j] >= MIN);
            assert(v[j] <= MAX);
        }
    }

    std::cout << "  PASSED\n";
}


void test_random_unit_vector() {
    std::cout << "Testing random_unit_vector...\n";

    for (int i = 0; i < 1000; ++i) {
        vec<3> v = random_unit_vector<3>();

        double length = norm(v);

        assert(approx(length, 1.0, 1e-7));
    }

    std::cout << "  PASSED\n";
}


// ============================================================
// Main
// ============================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "Running vector/matrix tests\n";
    std::cout << "========================================\n\n";

    test_vector_indexing();
    test_vector_size();
    test_vector_assignment();

    test_vector_addition();
    test_vector_addition_assignment();
    test_vector_subtraction();
    test_vector_subtraction_assignment();
    test_vector_negation();

    test_vector_scalar_multiplication();
    test_vector_scalar_multiplication_assignment();
    test_vector_scalar_division();

    test_dot_product();
    test_norm_squared();
    test_norm();
    test_cross_product();

    test_unit_vector();
    test_reflect();
    test_near_zero();

    test_matrix_size();
    test_matrix_indexing();
    test_matrix_assignment();

    test_matrix_addition();
    test_matrix_subtraction();
    test_matrix_negation();
    test_matrix_scalar_multiplication();
    test_matrix_scalar_division();

    test_matrix_multiplication();

    test_random_range();
    test_random_unit_vector();

    std::cout << "\n========================================\n";
    std::cout << "ALL TESTS PASSED\n";
    std::cout << "========================================\n";

    return 0;
}