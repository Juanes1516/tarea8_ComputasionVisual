#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include "vec3.h"
#include "ray.h"
#include "interval.h"
#include "rtweekend.h"

#include <iostream>
#include <fstream>

class camera {
public:
    
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;

    
    double vfov = 90;              // Campo de visi�n vertical en grados.
    point3 lookfrom = point3(0, 0, 0);   // Punto de la c�mara.
    point3 lookat = point3(0, 0, -1);    // Punto al que se dirige la c�mara.
    vec3 vup = vec3(0, 1, 0);      // Vector "up" de la c�mara.
    double defocus_angle = 0;      // �ngulo del cono de variaci�n 
    double focus_dist = 10;        // Distancia al plano de enfoque.

    void render(const hittable& world, std::ostream& out = std::cout) {
        initialize();

        out << "P3\n" << image_width << " " << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; s++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(out, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                 \n";
    }

    void render_to_file(const hittable& world, const std::string& filename) {
        std::ofstream out_file(filename);
        if (!out_file.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filename << " para escritura.\n";
            return;
        }
        render(world, out_file);
        out_file.close();
        std::clog << "Imagen guardada en '" << filename << "'\n";
    }

private:
    int image_height;
    double pixel_samples_scale;
    point3 center;         
    point3 pixel00_loc;    
    vec3 pixel_delta_u;    
    vec3 pixel_delta_v;   
    // Bases de la c�mara.
    vec3 u, v, w;
    // Vectores para la base del disco de defocus.
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        if (image_height < 1)
            image_height = 1;
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Calcula la distancia focal como la longitud entre lookfrom y lookat.
        auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calcula la base de la c�mara.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectores del viewport.
        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * (-v);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calcula el radio del disco de defocus a partir del �ngulo.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);
        point3 ray_origin = center;
        if (defocus_angle > 0)
            ray_origin = defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
       
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Retorna un punto aleatorio en el disco de defocus.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
            return color(0, 0, 0);
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif
