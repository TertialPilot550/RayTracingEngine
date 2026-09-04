#pragma once
#include "../../../../main.hpp"


class Lambertian : public Material {
  public:
    explicit Lambertian(const color& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    explicit Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

    bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered)
    const override {
        auto scatter_direction = rec.normal + random_unit_vector<4>();

        // Catch degenerate scatter direction
        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;

        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.t_coords[0], rec.t_coords[1], rec.p);
        return true;
    }

  virtual color at(int u, int v, const point& p) const {
      return tex->value(u, v, p);
  }


  private:
    std::shared_ptr<Texture> tex;
};