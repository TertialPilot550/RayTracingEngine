#pragma once
#include "../../../main.hpp"

class DiffuseLight : public Material {
  public:
    DiffuseLight(std::shared_ptr<Texture> tex) : tex(tex) {}
    DiffuseLight(const color& emit) : tex(std::make_shared<SolidColor>(emit)) {}

    color emitted(double u, double v, const point& p) const override {
        return tex->value(u, v, p);
    }

  private:
    std::shared_ptr<Texture> tex;
};