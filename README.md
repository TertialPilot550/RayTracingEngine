# <center> Ray Tracing Engine </center>
<< I want to make sure to try and describe the whole feature list. This document is for my understanding as well as for presenting to audiences about the project. Documentation: Organize the project and get it under control. Complete design.md and create doxygen comments covering all of the code. Refactor. {Potentially: add cpp files to clean up the headers} >>

### <center>Table of Contents</center>
1. src/camera
    - camera controls
    - camera object


- objects
    - collidables
    - primitives    
        - Sphere
        - Triangles
        - Quads
    - composites
        - tiny_obj_loader
    - surfaces

- scenes
    - scene object
    - demos


- accel
    - TaskMaster



- util
    - ray
    - vector
    - rgb/image
    - noise




# Camera
```c++
// TODO ADD CAMERA CONTROLS: UPDATE THE CLASS TO BE MORE EXPLICIT IN ALLOWING FEATURE CONFIGURATION: TURNING STUFF ON AND OFF
```

```c++
class CameraRig {

    public:

    Image* img_buffer;
    CameraRig();
    Image& capture(Scene& s);

    private:

    // Impure
    void render(Scene& s);

    // Pure 
    color process_ray(const Ray& r, int depth, Scene& s);

    // Write to the image buffer (impure)
    void write_color(const rgb& rgb, int x, int y);

    // Pure 
    vec3 sample_square() const;

    // Pure
    point defocus_disk_sample(CamControls& controls) const;
    // Pure
    Ray get_ray_for_pixel(int i, int j, CamControls& controls) const;

    // Pure
    rgb sample_for_pixel_color(int x, int y, Scene& s);

    // Pure
    rgb color_correction_to_rgb(const color& pixel_color);

};
```

- Configurable Position, Orientation, and FOV
- Anti-Aliasing
- Defocus Blur / Depth of Field
- {Motion Blur}

Performance:
- Spatial Subdivision (Pixel Chunking based on image width)
- Parrallel Rendering



# Objects

## Geometric Primitives

> Collidables



1. Spheres
2. Triangles
3. Quads

### Predefined Triangle Meshes (.obj)




## Surfaces

### Materials
- Dielectric
- Diffuse Light
- Lambertian
- Metal
### Textures
- Checker
- Image
- Noise
- Solid Color



# Scenes 
```c++
class Scene {

    public:
    CamControls controls;
    CollisionList objects;
    color background;

    // Default to a kind of sky blue
    Scene() : controls(CamControls()), background(176, 252, 255) {}

    
};
```


# Acceleration

# Utilities
