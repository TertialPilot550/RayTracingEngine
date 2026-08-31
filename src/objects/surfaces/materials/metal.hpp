#pragma once
#include "../../../main.hpp"

class Metal : public Material {
  public:
    Metal(const color& albedo, double fuzziness) : albedo(albedo), fuzziness(fuzziness) {}

    bool scatter(const Ray& r_in, const CollisionRecord& rec, color& attenuation, Ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzziness * random_unit_vector());
        scattered = Ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzziness;
};