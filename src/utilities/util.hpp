#pragma once
#include <random>
#include <limits>

// Constants

/**
 * @brief the value of infinity
 */
const double infinity = std::numeric_limits<double>::infinity();
/**
 * @brief the value of pi
 */
const double pi = 3.1415926535897932385;

/**
 * @brief Conversion from degrees to radians
 */
double degrees_to_radians(double degrees);

/**
 * @brief Conversion from radian to degress
 */
double radians_to_degrees(double rad);

/**
 * @brief Color correction utility
 * @details The justification for this can be found in ray tracing on one weekend.
 */
double linear_to_gamma(double linear_component);

/**
 * @brief Random double in the range [0, 1)
 * @details thread_local random state to allow parralelization of tasks dependent on random numbers
 */
double random_double();

/**
 * @brief Random double in an arbitrary range [min, max)
 */
double random_double(double min, double max);

/**
 * @brief Random int in an arbitrary range [min, max)
 */
int random_int(int min, int max);