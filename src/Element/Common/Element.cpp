#include "Element.h"

#include "Geometry/Line.h"

#include <algorithm>
#include <numeric>

namespace FE {
    Element::Element(const std::vector<std::reference_wrapper<Node> > &nodes_p, const uint id_p,
                     Type type_p) : id(id_p), type(type_p), nodes(nodes_p) {
        updateBoundingBox();
    }

    Element::Element(uint id_p, Element::Type type_p) : id(id_p),
                                                        type(type_p) {
    }

    bool Element::operator==(const Element &other) const {
        if (nodes.size() != other.nodes.size())
            return false;
        for (std::vector<std::reference_wrapper<Node>>::size_type i = 0; i < nodes.size(); ++i) {
            if (nodes[i].get() != other.nodes[i].get())
                return false;
        }
        return true;
    }

    double Element::find_biggest_x() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.x,
                               [](double max, const Node &next) {
                                   return std::max(max, next.coords.x);
                               });
    }

    double Element::find_biggest_y() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.y,
                               [](double max, const Node &next) {
                                   return std::max(max, next.coords.y);
                               });
    }

    double Element::find_biggest_z() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.z,
                               [](double max, const Node &next) {
                                   return std::max(max, next.coords.z);
                               });
    }

    double Element::find_smallest_x() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.x,
                               [](double min, const Node &next) {
                                   return std::min(min, next.coords.x);
                               });
    }

    double Element::find_smallest_y() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.y,
                               [](double min, const Node &next) {
                                   return std::min(min, next.coords.y);
                               });
    }

    double Element::find_smallest_z() const {
        return std::accumulate(nodes.cbegin(), nodes.cend(), nodes[0].get().coords.z,
                               [](double min, const Node &next) {
                                   return std::min(min, next.coords.z);
                               });
    }

    bool Element::is_in_x_range(const Node &node) const {
        return find_smallest_x() <= node.coords.x && node.coords.x <= find_biggest_x();
    }

    bool Element::is_in_y_range(const Node &node) const {
        return find_smallest_y() <= node.coords.y && node.coords.y <= find_biggest_y();
    }

    uint Element::getId() const {
        return id;
    }

    Element::Type Element::getType() const {
        return type;
    }

    const Node &Element::getNodeByIndex(uint index) const {
        return nodes[index];
    }

    Coords Element::interpolate(const Node &node) const {
        std::vector<double> params;
        double total = 0;
        std::transform(nodes.cbegin(), nodes.cend(), std::back_inserter(params), [&node, &total](const Node &vertice) {
            auto len = Line(vertice, node).squareLength();
            total += len;
            return len;
        });
        std::transform(params.begin(), params.end(), params.begin(), [&total](double len) {
            return 1 - len / total;
        });
        std::vector<Coords> weightedVectorField;
        std::transform(params.begin(), params.end(), nodes.cbegin(), std::back_inserter(weightedVectorField),
                       [](double p, const Node &vertice) {
                           auto &[x, y, z] = vertice.vector_field.coords;
                           return Coords{x * p, y * p, z * p};
                       });
        auto res = std::accumulate(weightedVectorField.cbegin(), weightedVectorField.cend(), Coords{0, 0},
                                   [](const Coords &accumulator, const Coords &vector) {
                                       auto &[acc_x, acc_y, acc_z] = accumulator;
                                       auto &[vec_x, vec_y, vec_z] = vector;
                                       return Coords{acc_x + vec_x, acc_y + vec_y, acc_z + vec_z};
                                   });
        return res;
    }

    void Element::updateBoundingBox() {
        auto maxX = find_biggest_x();
        auto minX = find_smallest_x();
        auto maxY = find_biggest_y();
        auto minY = find_smallest_y();
        auto maxZ = find_biggest_z();
        auto minZ = find_smallest_z();
        boundingBox = BoundingBox({minX, maxX}, {minY, maxY}, {minZ, maxZ});
    }
}
