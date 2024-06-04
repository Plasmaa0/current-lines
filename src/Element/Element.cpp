#include "Element.h"

#include "Geometry/Line.h"
#include "QuadrangleElement.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>

Element::Element(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p, Type type_p) : nodes(nodes_p),
                                                                                                     id(id_p),
                                                                                                     type(type_p) {
    sort_nodes();
}

Element::Element(uint id_p, Element::Type type_p) : id(id_p),
                                                    type(type_p) {}

struct NodeComparatorLess {
    Coords center;

    explicit NodeComparatorLess(const Coords &center_p) : center(center_p) {}

    bool operator()(const Node &first, const Node &second) const {
        auto &a = first.coords;
        auto &b = second.coords;
        // less, clockwise, https://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
        if (a.x - center.x >= 0 && b.x - center.x < 0)
            return true;
        if (a.x - center.x < 0 && b.x - center.x >= 0)
            return false;
        if (a.x - center.x == 0 && b.x - center.x == 0) {
            if (a.y - center.y >= 0 || b.y - center.y >= 0)
                return a.y > b.y;
            return b.y > a.y;
        }

        // compute the cross product of vectors (center -> a) x (center -> b)
        double det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
        if (det < 0)
            return true;
        if (det > 0)
            return false;

        // points a and b are on the same line from the center
        // check which point is closer to the center
        double d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
        double d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
        return d1 > d2;
    }
};

struct NodeComparatorGreater {
    NodeComparatorLess lessComparator;

    explicit NodeComparatorGreater(const Coords &center_p) : lessComparator(NodeComparatorLess(center_p)) {}

    bool operator()(const Node &a, const Node &b) const {
        return not lessComparator(a, b);
    }
};

void Element::sort_nodes() { // FIXME не универсально
    auto nodes_i = static_cast<double>(nodes.size());
    auto center = std::accumulate(nodes.cbegin(), nodes.cend(), Coords{0, 0, 0},
                                  [nodes_i](Coords acc, const Node &node) {
                                      return Coords{acc.x + node.coords.x / nodes_i, acc.x + node.coords.y / nodes_i,
                                                    acc.x + node.coords.z / nodes_i};
                                  });
    std::sort(nodes.begin(), nodes.end(), NodeComparatorLess(center));
    return;
    while (true) {
        if (nodes[0].get().coords.x > nodes[1].get().coords.x) {
            std::swap(nodes[0], nodes[1]);
        }

        if (nodes[0].get().coords.x == nodes[1].get().coords.x && nodes[0].get().coords.y < nodes[1].get().coords.y) {
            std::swap(nodes[0], nodes[1]);
        }

        if (nodes[1].get().coords.y < nodes[2].get().coords.y) {
            std::swap(nodes[1], nodes[2]);
        }
        if (nodes[2].get().coords.x < nodes[3].get().coords.x) {
            std::swap(nodes[2], nodes[3]);
        }
        if (nodes[3].get().coords.y > nodes[0].get().coords.y) {
            std::swap(nodes[3], nodes[0]);
        }

        if (nodes[0].get().coords.x <= nodes[1].get().coords.x && nodes[1].get().coords.y >= nodes[2].get().coords.y &&
            nodes[2].get().coords.x >= nodes[3].get().coords.x && nodes[3].get().coords.y <= nodes[0].get().coords.y) {
            break;
        }
    }
}

bool Element::operator==(const Element &other) const {
    if (nodes.size() != other.nodes.size())
        return false;
    for (int i = 0; i < nodes.size(); ++i) {
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

bool Element::is_in_x_range(const Node &node) const {
    return find_smallest_x() <= node.coords.x && node.coords.x <= find_biggest_x();
}

bool Element::is_in_y_range(const Node &node) const {
    return find_smallest_y() <= node.coords.y && node.coords.y <= find_biggest_y();
}

uint Element::getId() const {
    return id;
}

void Element::setId(uint id_p) {
    id = id_p;
}

Element::Type Element::getType() const {
    return type;
}

void Element::setType(Element::Type type_p) {
    type = type_p;
}

const Node &Element::getNodeByIndex(uint index) const {
    return nodes[index];
}

bool Element::contains_node(const Node &node) const {  // FIXME не универсально
    return contains_node_raycasting(node);
}

bool Element::contains_node_cross_product(const Node &node) const {  // FIXME не универсально
    auto inRange = is_in_x_range(node) and is_in_y_range(node);
    if (not inRange)
        return false;
    for (int i = 0; i < nodes.size() - 1; ++i)
        if (Line(nodes[i], nodes[i + 1]).findPosition(node) < 0)
            return false;
    return Line(nodes.back(), nodes.front()).findPosition(node) >= 0;
//    return Line(nodes[0], nodes[1]).findPosition(node) >= 0.0 and
//           Line(nodes[1], nodes[2]).findPosition(node) >= 0.0 and
//           Line(nodes[2], nodes[3]).findPosition(node) >= 0.0 and
//           Line(nodes[3], nodes[0]).findPosition(node) >= 0.0;
}

bool Element::contains_node_raycasting(const Node &node) const {
    // ray-casting algorithm based on
    // https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html

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

