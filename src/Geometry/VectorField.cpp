#include "VectorField.h"

VectorField::VectorField(const Coords &coords_p)
    : coords(coords_p.x, coords_p.y, coords_p.z), coords_normalize(coords_p.get_normalize()) {
}

void VectorField::set_coords(const Coords &coords_p) {
    coords = coords_p;
    coords_normalize = coords_p.get_normalize();
}

bool VectorField::operator==(const VectorField &other) const {
    return coords == other.coords;
}

std::ostream &operator<<(std::ostream &os, const VectorField &obj) {
    return os << std::format("VectorField({})", obj.coords);
}
