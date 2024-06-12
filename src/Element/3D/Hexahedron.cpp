#include "Hexahedron.h"
#include <Geometry/Plane.h>


namespace FE::Volumetric {
    bool Hexahedron::contains_node(const Node &node) const {
        // смотрим снаружи, обходим против часовой стрелки
        // https://www.manpagez.com/info/gmsh/gmsh-2.4.0/gmsh_58.php#SEC58
        auto side1 = Plane(nodes[0].get(), nodes[1].get(), nodes[2].get()); // back
        auto side2 = Plane(nodes[4].get(), nodes[0].get(), nodes[7].get()); // left
        auto side3 = Plane(nodes[5].get(), nodes[4].get(), nodes[6].get()); // front
        auto side4 = Plane(nodes[1].get(), nodes[5].get(), nodes[2].get()); // right
        auto side5 = Plane(nodes[6].get(), nodes[7].get(), nodes[2].get()); // top
        auto side6 = Plane(nodes[1].get(), nodes[0].get(), nodes[5].get()); // bottom
        return side1.findPosition(node) >= 0 and
               side2.findPosition(node) >= 0 and
               side3.findPosition(node) >= 0 and
               side4.findPosition(node) >= 0 and
               side5.findPosition(node) >= 0 and
               side6.findPosition(node) >= 0;
    }

    Coords Hexahedron::interpolate(const Node &node) const {
        return Element::interpolate(node);
    }
}