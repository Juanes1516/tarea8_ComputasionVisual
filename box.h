#ifndef BOX_H
#define BOX_H

#include "rtweekend.h"
#include "hittable.h"

class box : public hittable {
public:
    box() {}

    
    box(const point3& p0, const point3& p1) : box_min(p0), box_max(p1) {}

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    point3 box_min;
    point3 box_max;
};

bool box::hit(const ray& r, interval ray_t, hit_record& rec) const {

    double t_min = ray_t.min;
    double t_max = ray_t.max;

    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (box_min[a] - r.origin()[a]) * invD;
        auto t1 = (box_max[a] - r.origin()[a]) * invD;

        if (invD < 0.0f)
            std::swap(t0, t1);

        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;

        if (t_max <= t_min)
            return false;
    }

    
    rec.t = t_min;
    rec.p = r.at(rec.t);

    
    vec3 outward_normal;
    auto min_dist = std::numeric_limits<double>::infinity();

    // Reviso cada cara para encontrar la más cercana al punto de intersección
    for (int i = 0; i < 3; i++) {
        double dist;

        // Cara con coordenada mínima en este eje
        dist = std::abs(rec.p[i] - box_min[i]);
        if (dist < min_dist) {
            min_dist = dist;
            outward_normal = vec3(0, 0, 0);
            outward_normal[i] = -1;
        }

        // Cara con coordenada máxima en este eje
        dist = std::abs(rec.p[i] - box_max[i]);
        if (dist < min_dist) {
            min_dist = dist;
            outward_normal = vec3(0, 0, 0);
            outward_normal[i] = 1;
        }
    }

    rec.set_face_normal(r, outward_normal);

    return true;
}

#endif
#pragma once
