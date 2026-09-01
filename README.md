# <center> Ray-Tracing Engine </center>

Class Project for CPRE 3360. Rendering engine implemented using ray tracing. 
See the feature list below. 

### <center>Table of Contents</center>
1. src/camera
    - CamControls
    - CameraRig

2. src/objects
    - collidables
        - CollisionObject
        - CollisionRecord
        - CollisionList
    - primitives    
        - Sphere
        - Triangle
        - Quad
    - composites
        - OBJModel
    - surfaces
        - Material
        - Texture

3. src/scenes
    - Scene

4. src/accel
    - TaskMaster

5. src/util
    - Ray
    - Interval
    - vec2
    - vec3
    - Perlin

# Feature List

## Camera

- Configurable Position, Orientation, and FOV
- Anti-Aliasing
- Defocus Blur / Depth of Field (10 points)
- {Motion Blur} (10 Points)

- Ray/Primitive Intersection
- Textured Primitives


Performance: See Acceleration
- Spatial Subdivision (Pixel Chunking based on image width)





## Objects

### Geometric Primitives
1. Spheres
2. Triangles
3. Quads (10 Points)

### Lists of Primitives

#### Predefined Triangle Meshes (.obj)


### Surfaces

#### Materials
- Dielectric
- DiffuseLight
- Lambertian
- Metal
#### Textures
- CheckerTexture
- ImageTexture
- NoiseTexture
- SolidColor

## Acceleration
- Parrallel Rendering (10 points)

## Utilies
- Perlin Noise (10 Points)

## Scenes 
#### Demo Scenes
- Sphere Demo
- Triangle Demo
- DiffuseLight Demo


# REQUIRED FEATURED REMAINING: Triangle Meshes, Bug Fixes
# CUMULATIVE POINT TOTAL: 40 {50}

--- 

# COMS 3360 PROJECT TODO LIST

---
### Finished
Ray Tracing in One Weekend: Volume One
- [X] A camera with configurable position, orientation, and field of view: Required
- [X] Anti-aliasing: Required
- [X] Ray/sphere intersections: Required
- [X] Defocus blur/depth of field: 10
- [X] Specular, diffuse, and dielectric materials

Independently Written
- [X] A spatial subdivision acceleration structure of your choice
- [X] Parallelization: 10
- [X] Ray/triangle intersections: Required 
- [X] Textured triangles: Required
- [X] The ability to load and render triangle meshes (file format(s) of your choice; may use third-party libraries for loading): Required


Ray Tracing in One Weekend: Volume Two
- [X] Perlin noise: 10                         
- [X] The ability to load textures: Required    
- [X] Textured spheres: Required                
- [X] Textured quads: 10                        
- [X] Emissive materials (lights): Required     

ALL REQUIRED FEATURES COMPLETE
POINTS SO FAR: 40 (50 if motion blur works)

---
### IN PROGRESS
- [ ] Transformations on objects/Object Instancing: 10

### NEXT
- [ ] Motion blur: 10                           <- Need to add motion in order to test this, and I also need to make sure that the ray intersection code for each shape properly handles motion and whatnot

### BACKBURNER
- [ ] Materials for Triangle Meshes

---
Theoretical Point Total before acceleration and animation wrapper: All Required + 60 points (40 points remaining)

### Acceleration
- [ ] Hybrid rendering with a GPU (depth buffer + ray tracing): 20
- [ ] GPU acceleration (GPU computing w/ e.g., CUDA): 20
- [ ] Importance Sampling: 15

---
Theoretical Point Total: 115

- [ ] Real good documentation / report

### Stretch Goals: GUI 
- [ ] STRETCH: Skeleton articulation system?
- [ ] STRETCH: Animation wrapper (monochrome cross hatching texture??)