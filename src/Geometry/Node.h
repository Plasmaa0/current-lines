#pragma once

#include <Geometry/VectorField.h>
#include <Geometry/Coords.h>
#include <Geometry/Node.h>
#include <Utils/Color.h>

#include <cstdlib>
#include <ostream>

class Node {
public:
    static const uint INVALID_ID = 0;
    uint id;
    VectorField vector_field;
    Coords coords;
    mutable Color color;

    Node(uint id_p, double x_p, double y_p, double z_p = 0);

    Node(uint id_p, const Coords &coords_p);

    [[nodiscard]] bool operator==(const Node &other) const;

    void set_coords(const Coords &new_coords);

    friend std::ostream &operator<<(std::ostream &os, const Node &obj);

    void set_color(const Color &new_color);
};

DEFINE_FORMATTER(Node)