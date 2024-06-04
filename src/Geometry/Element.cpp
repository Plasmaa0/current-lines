#include "Element.h"

#include "Line.h"

#include <algorithm>
#include <numeric>

Element::Element(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p, Type type_p) : nodes(nodes_p),
                                                                                                     id(id_p),
                                                                                                     type(type_p) {
    sort_nodes();
}

Element::Element(Node &node1, Node &node2, Node &node3, Node &node4, uint id_p, Type type_p) : id(id_p),
                                                                                               type(type_p) {
    nodes = {node1, node2, node3, node4};
    sort_nodes();
}

void Element::sort_nodes() {
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

bool Element::the_same(const Element &other) const {
    return nodes[0].get().the_same(other.nodes[0]) && nodes[1].get().the_same(other.nodes[1]) &&
           nodes[2].get().the_same(other.nodes[2]) && nodes[3].get().the_same(other.nodes[3]);
}

bool Element::operator==(const Element &other) const {
    return the_same(other);
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
#include <iostream>
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

bool Element::contains_node(const Node &node) const {
    return is_in_x_range(node) and is_in_y_range(node) and
            Line(nodes[0], nodes[1]).findPosition(node) >= 0.0 and
            Line(nodes[1], nodes[2]).findPosition(node) >= 0.0 and
            Line(nodes[2], nodes[3]).findPosition(node) >= 0.0 and
            Line(nodes[3], nodes[0]).findPosition(node) >= 0.0;
}

std::optional<CrossPoints> Element::find_cross_point_x(const Node &node) const {
    Line scan_line(node, Node(Node::INVALID_ID, node.coords.x + 1.0, node.coords.y));

    if (node.coords.x == find_smallest_x() || node.coords.x == find_biggest_x()) {
        return std::nullopt;
    }

    std::optional<Node> small = scan_line.linesIntersect(Line(nodes[3], nodes[0]));
    std::optional<Node> big = scan_line.linesIntersect(Line(nodes[1], nodes[2]));

    if (small.has_value() && big.has_value()) {
        return CrossPoints(small.value(), big.value());
    }

    return std::nullopt;
}

uint Element::getId() const {
    return id;
}

void Element::setId(uint id_p) {
    Element::id = id_p;
}

Element::Type Element::getType() const {
    return type;
}

void Element::setType(Element::Type type_p) {
    Element::type = type_p;
}

const Node &Element::getNodeByIndex(uint index) const {
    return nodes[index];
}

