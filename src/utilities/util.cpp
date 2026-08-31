#include "util.hpp"

double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}


double radians_to_degrees(double rad) {
    return (rad * 180.0)/pi;
}


double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}


double random_double() {

    thread_local std::mt19937 generator(std::random_device{}());
    thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);

    return distribution(generator);

}


double random_double(double min, double max) {
    return min + (max-min)*random_double();
}


int random_int(int min, int max) {
    // 1. Thread-local seed source and engine initialization (happens once per thread)
    thread_local std::random_device rd;
    thread_local std::mt19937 generator(rd());
    
    // 2. Define the distribution range
    std::uniform_int_distribution<int> distribution(min, max);
    
    // 3. Generate the number
    return distribution(generator);
}