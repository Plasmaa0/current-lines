#include "ElementFactory.h"

#include <utility>
#include "Element/2D/Triangle.h"
#include "Element/2D/Quadrangle.h"

#include "Element/3D/Tetrahedron.h"
#include "Element/3D/Hexahedron.h"

namespace FE {
    std::shared_ptr<Element>
    Factory::createElement(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p,
                           Element::Type type_p) {
        switch (type_p) {
            case (Element::Type::Triangle):
                return std::make_shared<Planar::Triangle>(nodes_p, id_p, type_p);
            case (Element::Type::Quadrangle):
                return std::make_shared<Planar::Quadrangle>(nodes_p, id_p, type_p);
            case (Element::Type::Tetrahedron):
                return std::make_shared<Volumetric::Tetrahedron>(nodes_p, id_p, type_p);
            case (Element::Type::Hexahedron):
//                std::cout << "hexahedron implementation uses generic interpolation algorithm" << std::endl;
                return std::make_shared<Volumetric::Hexahedron>(nodes_p, id_p, type_p);
            case (Element::Type::Prism):
                NOT_IMPLEMENTED;
                break;  //return std::make_shared<PrismElement>(nodes_p, id_p, type_p);
            case (Element::Type::Pyramid):
                NOT_IMPLEMENTED;
                break;  //return std::make_shared<PyramidElement>(nodes_p, id_p, type_p);
            default:
                std::unreachable();
        }
        std::unreachable();
    }
}