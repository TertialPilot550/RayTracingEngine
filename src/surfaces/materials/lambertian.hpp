#pragma once
#include "../../lead.hpp"
class Lambertian : public Material {
  public:
    Lambertian(const color& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.t_coords.f1, rec.t_coords.f2, rec.p);
        return true;
    }

  private:
    std::shared_ptr<Texture> tex;
};