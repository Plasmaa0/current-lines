#pragma once

#include <Geometry/Geometry.h>
#include <vector>
#include <cstdlib>
#include <functional>
#include <optional>
#include <ostream>

namespace FE {
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

        Element(const std::vector<std::reference_wrapper<Node> > &nodes_p, uint id_p, Type type_p);

        Element(uint id_p, Type type_p);

        [[nodiscard]] uint getId() const;

        [[nodiscard]] Type getType() const;

        [[nodiscard]] const Node &getNodeByIndex(uint index) const;

        [[nodiscard]] bool operator==(const Element &other) const;

        [[nodiscard]] bool is_in_x_range(const Node &node) const;

        [[nodiscard]] bool is_in_y_range(const Node &node) const;

        [[nodiscard]] virtual bool contains_node(const Node &node) const = 0;

        [[nodiscard]] inline const BoundingBox &getBoundingBox() const { return boundingBox; }

        // наивная интерполяция на основе вычисления расстояния до вершин
        [[nodiscard]] virtual Coords interpolate(const Node &node) const;

        friend std::ostream &operator<<(std::ostream &os, const Element &obj) {
            return os << std::format("Element(id={},nodes={})", obj.id, obj.nodes.size());
        }

    protected:
        uint id;
        Type type;
        // skip tags
        std::vector<std::reference_wrapper<Node> > nodes;

        BoundingBox boundingBox;

        void updateBoundingBox();

    private:
        [[nodiscard]] double find_biggest_x() const;

        [[nodiscard]] double find_biggest_y() const;

        [[nodiscard]] double find_biggest_z() const;

        [[nodiscard]] double find_smallest_x() const;

        [[nodiscard]] double find_smallest_y() const;

        [[nodiscard]] double find_smallest_z() const;
    };
}

DEFINE_FORMATTER(FE::Element)
