# Rendering Engine

Completed Features (Required):
- A camera with configurable position, orientation, and field of view
- Anti-aliasing
- Ray/sphere intersections
- Ray/triangle intersections
- The ability to load textures (file format(s) of your choice; may use third-party libraries)
- Textured spheres and triangles
- The ability to load and render triangle meshes (file format(s) of your choice; may use third-party libraries for loading)
- A spatial subdivision acceleration structure of your choice
- Specular, diffuse, and dielectric materials (per first volume of Ray Tracing in One Weekend series)
- Emissive materials (lights)

Completed Features (Points):
- [10] Quads
- [10] Perlin Noise 
- [10] Defocus Blur / Depth of Field
- [10] Parralelization
- ? [10] Object Instancing << STILL NEEDS TESTING >>
- ? [10] Motion Blur << STILL NEEDS TESTING >>

Point Total: 40 (60)


```c++
// Example usage of the camera object with a demo scene
int main() {
    CameraRig cam;
    Image img = cam.capture(demo());
    img.write("label");
}
```

## Scenes

```c++
/**
 * @brief Object which represents all of the context for a particular instance
 * of the ray tracing algorithm, including objects, background, and camera
 * details.
 * 
 * @details Contains a list of objects, a background, and camera controls.
 * Background color defaults to sky blue
 * 
 * @see CamControls, CollisionList, color
 */
class Scene {

    public:
    CamControls controls;
    InstanceList objects;
    color background_color = color(0.5, 0.7, 1.0);
    Scene() : controls(CamControls()), background_color(color(0.5, 0.7, 1.0)) {}

    color background_color(double u, double v, const point& p) {
        return background_color;
    }
    
};
```
## Objects

Abstract Objects

```c++
/**
 * @brief Abstract class representing a collidable object.
 */
 class CollisionObject {
    public:
    std::shared_ptr<Surface> surf;
    CollisionObject(std::shared_ptr<Surface> surf): surf(surf) {}

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const;
    virtual point2D get_tcoords(const point& p) const = 0;

};

class CollisionList : public CollisionObject {
  public:
    std::vector<std::shared_ptr<CollisionObject>> objects;

    CollisionList() : CollisionObject(NULL) {}
    CollisionList(std::shared_ptr<CollisionObject> object) : CollisionObject(NULL) { add(object); }

    void clear() { objects.clear(); }

    void add(std::shared_ptr<CollisionObject> object) {
        objects.push_back(object);
    }

    point2D get_tcoords(const point& p) const override {
        point2D res;
        res[0] = 0;
        res[1] = 0;
        return res;
    }

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        CollisionRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const override {
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff);
        }
    }

};

```


### Surfaces

A surface is an abstraction used in place of the 'Material' and 'Placeholder' classes together. It defined the behavior of light within the medium, and the reflection of light,
as well as the objects phsical appearance/color/texture.

```c++
class Surface {

    public:
    Surface(std::shared_ptr<Material> mat, std::shared_ptr<Texture> tex) : mat(mat), tex(tex) {}

    std::shared_ptr<Material> mat;
    std::shared_ptr<Texture> tex;

    color value(int u, int v, const point& p) const {
        if (!tex) return color();
        return tex->value(u,v,p);
    }

    color emitted(double u, double v, const point& p) const {
        if (!mat) return color();
        return mat->emitted(u, v, p);
    }

    bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered) const {
        if (!mat) return false;
        return mat->scatter(r_in, rec, attenuation, scattered);
    }

};
```
Available Material Types:
1. Dielectric
2. Diffuse Light
3. Lambertian
4. Metal

Available Texture Types:
1. Checkered 
2. Image
3. Noise
4. Solid Color


### Primitive Objects

Geometric primitives are assumed to be made up of a set of N points (the skeleton of the object), as well as some rules or relation for creating the 
object, potentially based on other data (for example, the sphere and triangle have fundamentally different computational approaches).

```c++
template <int N>
class GeometricPrimitive : public CollisionObject {

    public:

    template<typename... Args>
    GeometricPrimitive(std::shared_ptr<Surface> surface, Args&&... args) : surface(surface), skeleton(std::forward<Args>(args)...) {}

    virtual bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const = 0;
    virtual void rasterize(mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, color* color_buff);
    virtual point2D get_tcoords(const point& p) const = 0;

    point skeleton[N];

};
```
Available Primitive Types:
1. Triangle
2. Parallelogram
3. Sphere

### Composite Objects

More detailed objects can be created by building a CollisionList which contains individual primitives making up a larger structure. 
The following function: 
```c++
inline bool load_model(OBJModel& m, const char* filepath);
```
loads a triangle mesh (in .obj format) into a native representation using a CollisionList of Triangle primitives, with the help
of the tiny_obj_loader library.

(Stretch Goal): Instances Linked by Fixed Relational Transforms (rigging)


### Instances

```c++
class Instance : public CollisionObject {

    public:

    std::shared_ptr<CollisionObject> obj;
    std::shared_ptr<Surface> sur;
    mat<4,4> transform;

    Instance(std::shared_ptr<CollisionObject> obj, std::shared_ptr<Surface> sur, mat<4,4> transform) : CollisionObject(sur), obj(obj), sur(sur), transform(transform) {}

    bool hit(const Ray& r, Interval ray_t, CollisionRecord& rec) const override {
        return obj->hit(r, ray_t, rec);
    }

    point2D get_tcoords(const point& p) const override {
        return obj->get_tcoords(p);
    }

    void rasterize(int screen_size[2], mat<4,4>& viewport_matrix, mat<4,4>& projection_to_camera_matrix, double* depth_buff, rgb* color_buff) const {
        obj->rasterize(screen_size, viewport_matrix, projection_to_camera_matrix, depth_buff, color_buff);
    }

};

```
NOTE: InstanceList is a straightforward wrapper of an std::vector<Instance>, in analogy to CollisionObjects and CollisionLists


## Rendering


#### Ray Tracing


#### Hybrid


#### Depth Buffer


(Stretch Goal): GUI 