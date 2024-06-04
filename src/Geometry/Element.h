#pragma once

#include "Node.h"
#include "CrossPoints.h"
#include <vector>
#include <cstdlib>
#include <functional>
#include <optional>

class Element {
public:
    enum class Type {
        Triangle,
        Quadrangle,
        Tetrahedron,
        Hexahedron,
        Prism,
        Pyramid,
    };

    explicit Element(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p, Type type_p);

    Element(Node &node1, Node &node2, Node &node3, Node &node4, uint id_p, Type type_p);

    [[nodiscard]] uint getId() const;

    void setId(uint id_p);

    [[nodiscard]] Type getType() const;

    void setType(Type type_p);

    [[nodiscard]] const Node &getNodeByIndex(uint index) const;

    [[nodiscard]] bool the_same(const Element &other) const;

    [[nodiscard]] bool operator==(const Element &other) const;

    [[nodiscard]] double find_biggest_x() const;

    [[nodiscard]] double find_biggest_y() const;

    [[nodiscard]] double find_smallest_x() const;

    [[nodiscard]] double find_smallest_y() const;

    [[nodiscard]] bool is_in_x_range(const Node &node) const;

    [[nodiscard]] bool is_in_y_range(const Node &node) const;

    [[nodiscard]] bool contains_node(const Node &node) const;

    [[nodiscard]] std::optional<CrossPoints> find_cross_point_x(const Node &node) const;

private:
    uint id;
    Type type;
    // skip tags
    std::vector<std::reference_wrapper<Node>> nodes;

    void sort_nodes();


};