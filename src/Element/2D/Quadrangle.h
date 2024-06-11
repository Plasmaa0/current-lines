#pragma once

#include <memory>
#include <optional>

#include "Element/2D/Element2D.h"
#include "Geometry/Coords.h"
#include "Geometry/CrossPoints.h"
#include "Geometry/Node.h"
#include <vector>
#include <cstdlib>
#include <functional>


namespace FE::Planar {
    class Quadrangle : public Element2D {
    public:
        using Element2D::Element2D;

        // билинейная интерполяция
        [[nodiscard]] Coords interpolate(const Node &node) const override;

    private:
        [[nodiscard]] std::optional<CrossPoints> find_cross_point_x(const Node &node) const;
    };
}
