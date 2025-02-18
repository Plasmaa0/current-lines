#include "Node.h"

Node::Node(uint id_p, double x_p, double y_p, double z_p)
    : id(id_p),
      coords(x_p, y_p, z_p),
      vector_field(coords),
      color({0, 0, 0}) {
}

Node::Node(uint id_p, const Coords &coords_p) : id(id_p),
                                                vector_field(coords),
                                                coords(coords_p),
                                                color({0, 0, 0}) {
}

bool Node::operator==(const Node &other) const {
    return id == other.id;
}

void Node::set_coords(const Coords &new_coords) {
    coords.set_coords(new_coords.x, new_coords.y);
}

void Node::set_color(const Color &new_color) {
    color = new_color;
}

std::ostream &operator<<(std::ostream &os, const Node &obj) {
    return os << std::format("Node(id: {}, {}, {}, {})", obj.id, obj.coords, obj.vector_field, obj.color);
}
