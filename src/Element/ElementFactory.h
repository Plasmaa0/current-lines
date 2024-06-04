#pragma once

#include <Element/Element.h>

#include <memory>


class ElementFactory {
public:
    static std::shared_ptr<Element> createElement(const std::vector<std::reference_wrapper<Node>> &nodes_p,
                                                  uint id_p,
                                                  Element::Type type_p);
};
