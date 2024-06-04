#pragma once

#include <Geometry/VectorField.h>
#include <Geometry/Coords.h>
#include <Geometry/Node.h>
#include <Utils/Color.h>

#include <cstdlib>

class Node {
public:
    static const uint INVALID_ID = 0;
    uint id;
    VectorField vector_field;
    Coords coords;
    Colors color;

    Node(uint id_p, double x_p, double y_p, double z_p=0);

    Node(uint id_p, const Coords& coords_p);

    [[nodiscard]] bool operator==(const Node &other) const;

    void set_coords(const Coords& new_coords);

    void set_color(Colors new_color);
};

