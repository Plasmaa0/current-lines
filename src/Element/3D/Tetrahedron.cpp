#include "Tetrahedron.h"
#include <Geometry/Plane.h>

namespace FE::Volumetric {

    bool Tetrahedron::contains_node(const Node &node) const {
        // смотрим снаружи, обходим против часовой стрелки
        // https://www.manpagez.com/info/gmsh/gmsh-2.4.0/gmsh_58.php#SEC58
        auto side1 = Plane(nodes[0].get(), nodes[1].get(), nodes[2].get());
        auto side2 = Plane(nodes[1].get(), nodes[3].get(), nodes[2].get());
        auto side3 = Plane(nodes[3].get(), nodes[0].get(), nodes[2].get());
        auto side4 = Plane(nodes[1].get(), nodes[0].get(), nodes[3].get());
        return side1.findPosition(node) >= 0 and
               side2.findPosition(node) >= 0 and
               side3.findPosition(node) >= 0 and
               side4.findPosition(node) >= 0;
    }

    Coords Tetrahedron::interpolate(const Node &p) const {
        // https://stackoverflow.com/questions/38545520/barycentric-coordinates-of-a-tetrahedron
        auto &a = nodes[0].get().coords;
        auto &b = nodes[1].get().coords;
        auto &c = nodes[2].get().coords;
        auto &d = nodes[3].get().coords;

        auto vap = p.coords - a;
        auto vbp = p.coords - b;

        auto vab = b - a;
        auto vac = c - a;
        auto vad = d - a;

        auto vbc = c - b;
        auto vbd = d - b;

        auto va6 = ScalarTripleProduct(vbp, vbd, vbc);
        auto vb6 = ScalarTripleProduct(vap, vac, vad);
        auto vc6 = ScalarTripleProduct(vap, vad, vab);
        auto vd6 = ScalarTripleProduct(vap, vab, vac);
        auto v6 = 1 / ScalarTripleProduct(vab, vac, vad);

        auto bary1 = va6 * v6;
        auto bary2 = vb6 * v6;
        auto bary3 = vc6 * v6;
        auto bary4 = vd6 * v6;
        auto res = (nodes[0].get().vector_field.coords * bary1) +
                   (nodes[1].get().vector_field.coords * bary2) +
                   (nodes[2].get().vector_field.coords * bary3) +
                   (nodes[2].get().vector_field.coords * bary4);
        return res;
    }
}
