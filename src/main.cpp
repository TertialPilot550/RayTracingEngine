#include "lead.hpp"
#include "scenes/triangle_demo.hpp"
#include "scenes/sphere_demo.hpp"
#include "scenes/light_demo.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "./tiny_obj_loader/tiny_obj_loader.h"

const std::string output_directory = "./out";

using Job = std::pair<std::string, Scene>;


/**
 * @brief Called by main(), and defines which scenes should be rendered when the program is run.
 * @details Helper function which allows the programmed to quickly switch which scenes
 * should be rendered by the program, and in which order when run. 
 */
std::vector<Job> build_jobs() {
    std::vector<Job> job_list; 

    // job_list.emplace_back(Job("TriangleDemo", triangle_demo()));
    // job_list.emplace_back(Job("SphereDemo", sphere_demo()));
    job_list.emplace_back(Job("LightDemo", light_demo()));

    return job_list;
}



/**
 * @brief main() 
 * @details Entry point for the program. Executes all jobs definied in
 * the build_jobs() function also located in main.cpp.
 */
int main() {

    auto job_list = build_jobs();
    std::cout << "RayTracingEngine :: Initialized with " << job_list.size() << " jobs" << std::endl;
    CameraRig cam;
    int jobs_fin = 0;
    for (auto& job : job_list) {
        png::image<png::rgb_pixel>& image = cam.capture(job.second);
        std::string lbl = output_directory + "/" + job.first + ".png";
        image.write(lbl);
        std::cout << "\tCompleted Job ("  << ++jobs_fin << "): [" << job.first << "]\n";
    }

    return 0;

}