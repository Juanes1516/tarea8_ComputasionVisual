#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "metal.h"
#include "box.h"

#include <fstream>
#include <memory>
using std::make_shared;

int main() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> obj_material;
                // Decide aleatoriamente si agregar esfera o box.
                if (random_double() < 0.5) {
                    // Agregar esfera.
                    if (choose_mat < 0.8) {
                        auto albedo = color::random() * color::random();
                        obj_material = make_shared<lambertian>(albedo);
                        world.add(make_shared<sphere>(center, 0.2, obj_material));
                    }
                    else if (choose_mat < 0.95) {
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        obj_material = make_shared<metal>(albedo, fuzz);
                        world.add(make_shared<sphere>(center, 0.2, obj_material));
                    }
                    else {
                        obj_material = make_shared<dielectric>(1.5);
                        world.add(make_shared<sphere>(center, 0.2, obj_material));
                    }
                }
                else {
                    // Agregar box.
                    double box_size = 0.4;
                    point3 box_min = center - vec3(box_size / 2, box_size / 2, box_size / 2);
                    point3 box_max = center + vec3(box_size / 2, box_size / 2, box_size / 2);

                    if (choose_mat < 0.8) {
                        auto albedo = color::random() * color::random();
                        obj_material = make_shared<lambertian>(albedo);
                        world.add(make_shared<box>(box_min, box_max, obj_material));
                    }
                    else if (choose_mat < 0.95) {
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        obj_material = make_shared<metal>(albedo, fuzz);
                        world.add(make_shared<box>(box_min, box_max, obj_material));
                    }
                    else {
                        obj_material = make_shared<dielectric>(1.5);
                        world.add(make_shared<box>(box_min, box_max, obj_material));
                    }
                }
            }
        }
    }

    // Objetos fijos adicionales:

    auto material1 = make_shared<dielectric>(1.5);
    // Box centrado en (0,1,0) con tamaño 2x2x2.
    world.add(make_shared<box>(point3(-1, 0, -1), point3(1, 2, 1), material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Configuración de la cámara.
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 25;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render_to_file(world, "imagen.ppm");

    return 0;
}
