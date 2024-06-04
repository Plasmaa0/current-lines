#pragma once

#include <Element/Element.h>
#include <Geometry/Coords.h>
#include <Geometry/Node.h>

class GenericElement : public Element {
public:
    using Element::Element;

    [[nodiscard]] Coords interpolate(const Node &node) const override;

};
