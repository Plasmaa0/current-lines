#include "VectorField.h"

VectorField::VectorField(const Coords &coords_p)
        : coords(coords_p.x, coords_p.y), coords_normalize(coords_p.get_normalize()) {}

bool VectorField::the_same(const VectorField &other) const {
    return coords.the_same(other.coords);
}

void VectorField::set_coords(const Coords &coords_p) {
    coords = coords_p;
    coords_normalize = coords_p.get_normalize();
}
