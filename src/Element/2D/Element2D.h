#pragma once

#include "Element/Common/Element.h"

namespace FE::Planar {

    class Element2D : public Element{

        using Element::Element;
    public:
        [[nodiscard]] bool contains_node(const Node &node) const override;

    private:
        [[nodiscard]] bool contains_node_cross_product(const Node &node) const;

        [[nodiscard]] bool contains_node_raycasting(const Node &node) const;
    };

}