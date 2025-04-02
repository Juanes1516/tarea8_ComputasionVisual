#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "box.h"

#include <fstream>  

int main() {
    hittable_list world;

    //world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    //world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Cubo pequeño centrado en (0, 0, -1)
    point3 min1(-1, -1, -2.2);
    point3 max1(1, 1, 0.2);
    world.add(make_shared<box>(min1, max1));

    // Cubo grande para el "suelo"
    point3 min2(-100, -100.5, -100);
    point3 max2(100, -100, 100);
    world.add(make_shared<box>(min2, max2));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    //cam.render(world);
    cam.render_to_file(world, "imagen.ppm");
}
