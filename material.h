#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "vec3.h"
#include "ray.h"

// Clase abstracta para materiales.
class material {
public:
    virtual ~material() = default;
    // El método scatter produce el rayo dispersado y la atenuación (albedo).
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

// Material lambertiano (difuso).
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
private:
    color albedo;
};

// Material dielectric (vidrio) que refracta cuando es posible y refleja en caso de
// total internal reflection.
class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0);
        // Si el rayo está en la cara frontal, ri = 1/indice; si no, ri = indice.
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        // Calcular cos_theta del ángulo entre el rayo entrante y la normal.
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        // Calcular sin_theta utilizando la relación trigonométrica.
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        // Determinar si se da la reflexión total interna.
        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;


        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            // Si TIR ocurre, se refleja.
            direction = reflect(unit_direction, rec.normal);
        else
            // Caso normal, se refracta.
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }
private:
    double refraction_index;
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif  // MATERIAL_H
