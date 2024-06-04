#include "GenericElement.h"
#include <Geometry/Line.h>
#include <algorithm>
#include <numeric>

Coords GenericElement::interpolate(const Node &node) const {
    std::vector<double> params;
    double total = 0;
    std::transform(nodes.cbegin(), nodes.cend(), std::back_inserter(params), [&node, &total](const Node &vertice) {
        auto len = Line(vertice, node).squareLength();
        total += len;
        return len;
    });
    std::transform(params.begin(), params.end(), params.begin(), [&total](double len) {
        return len / total;
    });
    std::vector<Coords> weightedVectorField;
    std::transform(params.begin(), params.end(), nodes.cbegin(), std::back_inserter(weightedVectorField),
                   [](double p, const Node &vertice) {
                       auto &[x, y, z] = vertice.vector_field.coords;
                       return Coords{x * p, y * p, z * p};
                   });
    auto res = std::accumulate(weightedVectorField.cbegin(), weightedVectorField.cend(), Coords{0, 0},
                               [](Coords accumulator, const Coords &vector) {
                                   auto &[acc_x, acc_y, acc_z] = accumulator;
                                   auto &[vec_x, vec_y, vec_z] = vector;
                                   return Coords{acc_x + vec_x, acc_y + vec_y, acc_z + vec_z};
                               });
    return res;
}
