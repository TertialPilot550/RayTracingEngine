#pragma once

#include "../../../main.hpp"

class Surface {

    public:
    Surface(std::shared_ptr<Material> mat, std::shared_ptr<Texture> tex) : mat(mat), tex(tex) {}

    explicit Surface(std::shared_ptr<Material> mat)
        : Surface(mat, nullptr) {}

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

inline std::shared_ptr<Surface> make_surface(std::shared_ptr<Material> mat,
                                             std::shared_ptr<Texture> tex = nullptr) {
    return std::make_shared<Surface>(std::move(mat), std::move(tex));
}