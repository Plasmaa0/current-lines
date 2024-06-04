#pragma once

#include <memory>
#include <optional>

#include <Element/Element.h>
#include <Geometry/Coords.h>
#include <Geometry/CrossPoints.h>
#include <Geometry/Node.h>
#include <vector>
#include <cstdlib>
#include <functional>

class QuadrangleElement : public Element {
    [[nodiscard]] std::optional<CrossPoints> find_cross_point_x(const Node &node) const;

public:
    using Element::Element;
    QuadrangleElement(Node &node1, Node &node2, Node &node3, Node &node4, uint id_p, Type type_p);

    [[nodiscard]] Coords interpolate(const Node &node) const override;

};
