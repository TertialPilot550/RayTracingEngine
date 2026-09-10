// TinyObjLoader
#define TINYOBJLOADER_IMPLEMENTATION
// Metal C++ 
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "main.hpp"

// #include <wx/wx.h>

const std::string output_directory = "./out";
using Job = std::pair<std::string, Scene>;

/**
 * @brief Called by main(), and defines which scenes should be rendered when the program is run.
 * @details Helper function which allows the programmed to quickly switch which scenes
 * should be rendered by the program, and in which order when run. 
 */
std::vector<Job> build_jobs() {
    std::vector<Job> job_list; 

    auto add_variants = [&job_list](const std::string& name, Scene scene) {
        Scene ray_tracing_scene = scene;
        ray_tracing_scene.controls.mode = CameraMode::RAY_TRACING;
        ray_tracing_scene.controls.accel_mode = AccelerationMode::PARALLEL;
        job_list.emplace_back(Job(name + "RayTracing", std::move(ray_tracing_scene)));

        Scene hybrid_scene = scene;
        hybrid_scene.controls.mode = CameraMode::HYBRID;
        hybrid_scene.controls.accel_mode = AccelerationMode::PARALLEL;
        job_list.emplace_back(Job(name + "Hybrid", std::move(hybrid_scene)));

        scene.controls.mode = CameraMode::DEPTH_BUFFER;
        scene.controls.accel_mode = AccelerationMode::PARALLEL;
        job_list.emplace_back(Job(name + "DepthBuffer", std::move(scene)));
    };

    add_variants("TriangleDemo", triangle_demo());
    add_variants("LightDemo", light_demo());
    add_variants("ModelDemo", model_demo());
    add_variants("MotionDemo", motion_demo());

    return job_list;
}


/**
 * @brief Render all demo scenes defined in build_jobs()
 */
void execute_jobs() {
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
}


/**
 * @brief main() 
 * @details Entry point for the program. Executes all jobs definied in
 * the build_jobs() function also located in main.cpp.
 */
int main() {

    execute_jobs();
    
    return 0;
}