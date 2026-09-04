# COMS 3360 PROJECT TODO LIST

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
- [X] Homogeneous coordinates

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
- TODO: add rasterization code for all primitives
- check the depth buffer document for edge cases (triangle edge sharing??)
- double check that the engine still works
- make sure rasterizing works

- [ ] Time based ray tracing/rendering, and Transformations on objects/Object Instancing: 10 (represented by 4d matrix)
- [ ] Motion blur: 10                           <- Need to add motion in order to test this, and I also need to make sure that the ray intersection code for each shape properly handles motion and whatnot

### NEXT
- [ ] Hybrid rendering with a GPU (depth buffer + ray tracing): 20
- [ ] GPU acceleration (GPU computing w/ e.g., CUDA): 20

### BACKBURNER
- [ ] Materials for Triangle Meshes (More general materials?)
- [ ] Importance Sampling: 15


Theoretical Point Total before acceleration and animation wrapper: All Required + 60 points (40 points remaining)

---
Theoretical Point Total: 115
- [ ] Real good documentation / report

### Stretch Goals: GUI 
- [ ] STRETCH: Skeleton articulation system?
- [ ] STRETCH: Animation wrapper (monochrome cross hatching texture??)