#pragma once
#include "Color.h"
#include "VectorField.h"

class Node {
public:
    int id;
    VectorField vector_field;
    Coords coords;
    Colors color;

    Node(int id_p, double x_p, double y_p, double z_p=0)
        : id(id_p), coords(x_p, y_p, z_p), vector_field(coords), color(Colors::GREEN) {}

    bool the_same(const Node& other) const {
        return id == other.id
            && coords.the_same(other.coords)
            && color == other.color;
    }

    void set_coords(const Coords& new_coords) {
        coords.set_coords(new_coords.x, new_coords.y);
    }

    void set_color(Colors new_color) {
        color = new_color;
    }
};



