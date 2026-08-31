#pragma once
#include "../main.hpp"

// image types courtesy of png++
using rgb_png = png::rgb_pixel;
using Image = png::image<rgb_png>;

/**
 * @brief Object which can rendering object to a 2D image using ray tracing algorithm
 * 
 * @details An abstraction for seperating rendering related code into it's own class, allowing 
 * users of the object to capture scenes using a simple interface.
 */
class CameraRig {

    public:

    /**
     * @brief Pointer to internal image buffer used by the camera 
     * @details Pointer to image buffer which the camera uses to produce
     * it's output images. Initialized to NULL.
     */
    Image* img_buffer = NULL;

    /**
     * @brief Default Constructor
     */
    CameraRig()  {}

    /**
     * @brief The only visible member function for a camera
     * 
     * @details Captures the objects in a given scene object to 
     * the image buffer. Handles making sure a valid image
     * buffer exists.
     */
    Image& capture(Scene& s);


    private: 

    /**
     * @name Impure Member Functions
     * Impure functions within the camera object, i.e. need to consider parralelization explicitely
     * when working with these functions
     */

    /**
     * @brief Use the ray tracing algorithm to render a scene to an image.
     * 
     * @details Uses an abstract computational helper (TaskMaster object) to 
     * perform various accerations for ray tracing. Uses the internal camera
     * controls for the scene to determine acceration and rendering parameters.
     * 
     * @see TaskMaster, CamControls
     */
    void render(Scene& s);

    /**
     * @brief Writes the provided rgb color to the provided pixel on
     * the internal image buffer. 
     * 
     * @details Helper method for render(Scene& s)
     */
    void write_color(const rgb& rgb, int x, int y);


    /**
     * @name Pure Member Functions
     * Pure functions within the camera object, i.e. easily parralelizable.
     */

    ///@{

    /**
     * @brief Recursively performs collision calculations for one ray.
     * 
     * @details Recursive function that handles collisions
     * for a particular ray. After a collision, another ray
     * will be sent out with depth-1, until depth is zero,
     * based on the material of the object's surface.
     */
    color process_ray(const Ray& r, int depth, Scene& s);


    /**
     * @brief Utility function used by get_ray_for_pixel()
     * 
     * @details Used to perform anti-aliasing. Returns a random
     * vec3 with zero z component, where x and y are between -0.5 and 0.5.
     */
    vec3 sample_square() const;


    /**
     * @brief Utility function used by get_ray_for_pixel()
     * 
     * @details Used to create defocus blur / depth of field.
     * Generates a ray starting not from the camera's center,
     * but from a point on a circle representing the theoretical
     * 'lens'.
     */
    point defocus_disk_sample(CamControls& controls) const;

    /**
     * @brief Utility function used by sample_for_pixel_color()
     * 
     * @details Used to generate a sample ray to determine color for a pixel
     * (x,y). This function implements both defocus blur/depth of field, as 
     * well as anti-aliasing
     */
    Ray get_ray_for_pixel(int i, int j, CamControls& controls) const;

    /**
     * @brief Utility function used by render()
     * 
     * @details Takes a number of samples for the pixel color,
     * for a pixel (x,y)
     */
    rgb sample_for_pixel_color(int x, int y, Scene& s);

    ///@}

};