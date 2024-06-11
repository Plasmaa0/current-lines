#pragma once

#include "Element.h"
#include "Utils/Nimpl.h"

#include <memory>

namespace FE {
    class Factory {
    public:
        static std::shared_ptr<Element> createElement(const std::vector<std::reference_wrapper<Node>> &nodes_p,
                                                      uint id_p,
                                                      Element::Type type_p);
    };
}