#include "Triangle.h"
#include <iostream>

namespace FE::Planar {
    Coords Triangle::interpolate(const Node &node) const {
        auto &vec = node.coords;
        auto &a = nodes[0].get().coords;
        auto &b = nodes[1].get().coords;
        auto &c = nodes[2].get().coords;
        // https://codeplea.com/triangular-interpolation
        auto den = 1 / ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));

        Coords baryCords{0, 0, 0};
        baryCords.x = ((b.y - c.y) * (vec.x - c.x) + (c.x - b.x) * (vec.y - c.y)) * den;
        baryCords.y = ((c.y - a.y) * (vec.x - c.x) + (a.x - c.x) * (vec.y - c.y)) * den;
        baryCords.z = 1 - baryCords.x - baryCords.y;

        auto res = (nodes[0].get().vector_field.coords * baryCords.x) + (nodes[1].get().vector_field.coords * baryCords.y) + (nodes[2].get().vector_field.coords * baryCords.z);
        return res;
    }
}