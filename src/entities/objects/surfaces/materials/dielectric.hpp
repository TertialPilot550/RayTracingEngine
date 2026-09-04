#pragma once
#include "../../../../main.hpp"

class Dielectric : public Material {
  public:
    Dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered)
    const override {
        attenuation = 1.0;
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        point unit_direction = unit_vector(r_in.direction());


        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        point direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = Ray(rec.p, direction, r_in.time());
        return true;
    }

    virtual color at(int u, int v, const point& p) {
        return color();
    }


  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};