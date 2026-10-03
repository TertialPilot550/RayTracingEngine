#pragma once

#include "../../../main.hpp"


class BSDFSample {
    point  wi;
    color f;                 // BSDF value (delta lobes include the 1/|cos| term)
    float pdf = 0;           // solid-angle pdf (or discrete probability for delta lobes)
    bool  isDelta = false;
    bool  isTransmission = false;
    float etap = 1.f;        // relative IOR along the sampled path (for Russian roulette etc.)
};


class BSDF {

    bool sample(point wo, point wi) {
        
    }

    color eval(point wo, point wi) {

    }

    // this might differ??? between bsfs???
    float pdf(point wo, float u1, float u2, float u3, BSDFSample* s) {

    }

};




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