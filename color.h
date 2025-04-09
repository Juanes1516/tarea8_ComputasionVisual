#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"
#include <cmath>  

using color = vec3;

// Función para convertir de espacio lineal a gamma (gamma 2: inversa es la raíz cuadrada)
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);
    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    // Aplicar corrección gamma a cada componente
    auto r = linear_to_gamma(pixel_color.x());
    auto g = linear_to_gamma(pixel_color.y());
    auto b = linear_to_gamma(pixel_color.z());

    // Traducir los valores en [0,1] a rango [0,255]
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Escribir el color en la salida
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif
#pragma once
