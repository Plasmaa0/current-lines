#include "Tetrahedron.h"

int sign(auto x) {
    return x > 0 ? 1 : -1;
}

bool SameSide(const Coords &v1, const Coords &v2, const Coords &v3, const Coords &v4, const Coords &p) {
    auto normal = (v2 - v1).cross(v3 - v1);
    auto dotV4 = normal.dot(v4 - v1);
    auto dotP = normal.dot(p - v1);
    // return dotV4 * dotP > 0;
    return sign(dotV4) == sign(dotP);
}


namespace FE::Volumetric {
    bool Tetrahedron::contains_node(const Node &node) const {
        auto &v1 = nodes[0].get().coords;
        auto &v2 = nodes[1].get().coords;
        auto &v3 = nodes[2].get().coords;
        auto &v4 = nodes[3].get().coords;
        return SameSide(v1, v2, v3, v4, node.coords) &&
               SameSide(v2, v3, v4, v1, node.coords) &&
               SameSide(v3, v4, v1, v2, node.coords) &&
               SameSide(v4, v1, v2, v3, node.coords);
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
