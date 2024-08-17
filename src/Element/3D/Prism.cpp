#include "Prism.h"
#include <Utils/Algorithms.h>

namespace FE::Volumetric {
    bool Prism::contains_node(const Node &node) const {
        const auto &v0 = nodes[0].get().coords;
        const auto &v1 = nodes[1].get().coords;
        const auto &v2 = nodes[2].get().coords;
        const auto &v3 = nodes[3].get().coords;
        const auto &v4 = nodes[4].get().coords;
        const auto &v5 = nodes[5].get().coords;
        return SameSide(v0, v2, v1, v3, node.coords) &&
               SameSide(v5, v3, v4, v2, node.coords) &&
               SameSide(v2, v5, v1, v3, node.coords) &&
               SameSide(v0, v3, v5, v1, node.coords) &&
               SameSide(v1, v4, v3, v2, node.coords);
    }

    Coords Prism::interpolate(const Node &node) const {
        return Element::interpolate(node);
    }
}
