#pragma once

#include "Element/Common/Element.h"
#include <Geometry/Geometry.h>

namespace FE::Volumetric {
    class Tetrahedron : public Element {
        using Element::Element;

    public:
        bool planeEquationMethod(const Node &node) const;

        [[nodiscard]] bool contains_node(const Node &node) const override;

        // барицентрические координаты
        [[nodiscard]] Coords interpolate(const Node &node) const override;
    };
} // Volumetric
