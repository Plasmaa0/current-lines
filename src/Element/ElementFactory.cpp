#include "ElementFactory.h"

#include <utility>
#include <Utils/Nimpl.h>
#include <Element/GenericElement.h>
//#include <Element/TriangleElement.h>
#include <Element/QuadrangleElement.h>
//#include <Element/TetrahedronElement.h>
//#include <Element/HexahedronElement.h>
//#include <Element/PrismElement.h>
//#include <Element/PyramidElement.h>


std::shared_ptr<Element>
ElementFactory::createElement(const std::vector<std::reference_wrapper<Node>> &nodes_p, uint id_p,
                              Element::Type type_p) {
    switch (type_p) { // TODO сделать LUT
        case (Element::Type::Triangle):
            return std::make_shared<GenericElement>(nodes_p, id_p, type_p);
            NOT_IMPLEMENTED; //return std::make_shared<TriangleElement>(nodes_p, id_p, type_p);
        case (Element::Type::Quadrangle):
            return std::make_shared<GenericElement>(nodes_p, id_p, type_p);
        case (Element::Type::Tetrahedron):
            NOT_IMPLEMENTED; //return std::make_shared<TetrahedronElement>(nodes_p, id_p, type_p);
        case (Element::Type::Hexahedron):
            NOT_IMPLEMENTED; //return std::make_shared<HexahedronElement>(nodes_p, id_p, type_p);
        case (Element::Type::Prism):
            NOT_IMPLEMENTED; //return std::make_shared<PrismElement>(nodes_p, id_p, type_p);
        case (Element::Type::Pyramid):
            NOT_IMPLEMENTED; //return std::make_shared<PyramidElement>(nodes_p, id_p, type_p);
        default:
            std::unreachable();
    }
    std::unreachable();
}