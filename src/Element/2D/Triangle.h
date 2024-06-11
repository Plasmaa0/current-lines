#pragma once

#include "Element/2D/Element2D.h"

#include "Geometry/Coords.h"
#include "Geometry/Node.h"

namespace FE::Planar {

    class Triangle : public Element2D {
    public:
        using Element2D::Element2D;

        // Метод на основе барицентрических координат
        Coords interpolate(const Node &node) const override;

    };

}
