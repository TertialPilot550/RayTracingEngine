COMS 3360 PROJECT TODO LIST

### Finished
- [X] A camera with configurable position, orientation, and field of view: Required
- [X] Anti-aliasing: Required
- [X] Ray/sphere intersections: Required
- [X] Defocus blur/depth of field: 10
- [X] A spatial subdivision acceleration structure of your choice
- [X] Specular, diffuse, and dielectric materials (per first volume of Ray Tracing in One Weekend series)

### IN PROGRESS

- [ ] Parallelization: 10

currently, almost everything should work, but I changed the random number generator to be independent per thread, and now it's going very slowly/not working not really 
sure which one. 

### Objects: Triangles, Textures, and Meshes
- [ ] The ability to load textures (file format(s) of your choice; may use third-party libraries): Required
- [ ] Ray/triangle intersections: Required
- [ ] Textured spheres and triangles: Required
- [ ] The ability to load and render triangle meshes (file format(s) of your choice; may use third-party libraries for loading): Required

### Light and Motion 
- [ ] Emissive materials (lights): Required
- [ ] Perlin noise: 10
- [ ] Motion blur: 10

### Acceleration
- [ ] Hybrid rendering with a GPU (depth buffer + ray tracing): 20
- [ ] GPU acceleration (GPU computing w/ e.g., CUDA): 20

### Stretch Goals: GUI 
- [ ] STRETCH: Animation wrapper (monochrome cross hatching texture??)