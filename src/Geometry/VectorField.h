#pragma once

#include <Geometry/Coords.h>

struct VectorField {
    Coords coords;
    Coords coords_normalize;

    explicit VectorField(const Coords &coords_p);

    [[nodiscard]] bool operator==(const VectorField &other) const;

    void set_coords(const Coords &coords_p);
};

