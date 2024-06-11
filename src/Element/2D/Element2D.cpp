#include "Element2D.h"

namespace FE::Planar {
    // FIXME не универсально
    bool Element2D::contains_node_cross_product(const Node &node) const {

        auto inRange = is_in_x_range(node) and is_in_y_range(node);
        if (not inRange)
            return false;
        for (int i = 0; i < nodes.size() - 1; ++i)
            if (Line(nodes[i], nodes[i + 1]).findPosition(node) < 0)
                return false;
        return Line(nodes.back(), nodes.front()).findPosition(node) >= 0;
    }

    // ray-casting algorithm based on
    // https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
    bool Element2D::contains_node_raycasting(const Node &node) const {
        auto &[x, y, z] = node.coords;

        auto inside = false;
        for (size_t i = 0, j = nodes.size() - 1; i < nodes.size(); j = i++) {
            auto &[xi, yi, zi] = nodes[i].get().coords;

            auto &[xj, yj, zj] = nodes[j].get().coords;

            auto intersect = ((yi > y) != (yj > y)) and (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
            if (intersect) inside = !inside;
        }
        return inside;
    }

    bool Element2D::contains_node(const Node &node) const {
        return contains_node_raycasting(node);
    }
}