#include "Coords.h"
#include <cmath>

Coords::Coords(double x_p, double y_p, double z_p) {
    x = x_p;
    y = y_p;
    z = z_p;
}

bool Coords::the_same(const Coords &other) const {
    return x == other.x && y == other.y && z == other.z;
}

void Coords::set_coords(double x_p, double y_p, double z_p) {
    x = x_p;
    y = y_p;
    z = z_p;
}

Coords Coords::get_normalize() const {
    double length = std::sqrt(x * x + y * y + z * z);
    return {x / length, y / length, z / length};
}
