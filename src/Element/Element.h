#pragma once

#include "Geometry/Node.h"
#include "Geometry/CrossPoints.h"
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

    Element(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p, Type type_p);

    Element(uint id_p, Type type_p);

    [[nodiscard]] uint getId() const;

    void setId(uint id_p);

    [[nodiscard]] Type getType() const;

    void setType(Type type_p);

    [[nodiscard]] const Node &getNodeByIndex(uint index) const;

    [[nodiscard]] bool operator==(const Element &other) const;

    [[nodiscard]] double find_biggest_x() const;

    [[nodiscard]] double find_biggest_y() const;

    [[nodiscard]] double find_smallest_x() const;

    [[nodiscard]] double find_smallest_y() const;

    [[nodiscard]] bool is_in_x_range(const Node &node) const;

    [[nodiscard]] bool is_in_y_range(const Node &node) const;

    [[nodiscard]] bool contains_node(const Node &node) const;
    [[nodiscard]] bool contains_node_cross_product(const Node &node) const;
    [[nodiscard]] bool contains_node_raycasting(const Node &node) const;

    [[nodiscard]] virtual Coords interpolate(const Node &node) const = 0;

protected:
    uint id;
    Type type;
    // skip tags
    std::vector<std::reference_wrapper<Node>> nodes;

    void sort_nodes();
};