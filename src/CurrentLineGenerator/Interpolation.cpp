#include "Interpolation.h"

#include <Geometry/Line.h>

std::optional<CrossPoints> first_interpolation(const Element &element, Node &baseNode) {
    if (auto cross_points = element.find_cross_point_x(baseNode)) {
        double t = Line(element.getNodeByIndex(3), cross_points->small).squareLength() /
                   Line(element.getNodeByIndex(0), element.getNodeByIndex(3)).squareLength();
        double tau = Line(element.getNodeByIndex(2), cross_points->big).squareLength() /
                     Line(element.getNodeByIndex(1), element.getNodeByIndex(2)).squareLength();

        double u_a = t * element.getNodeByIndex(0).vector_field.coords.x + (1.0 - t) * element.getNodeByIndex(3).vector_field.coords.x;
        double v_a = t * element.getNodeByIndex(0).vector_field.coords.y + (1.0 - t) * element.getNodeByIndex(3).vector_field.coords.y;

        double u_b = tau * element.getNodeByIndex(1).vector_field.coords.x + (1.0 - tau) * element.getNodeByIndex(2).vector_field.coords.x;
        double v_b = tau * element.getNodeByIndex(1).vector_field.coords.y + (1.0 - tau) * element.getNodeByIndex(2).vector_field.coords.y;

        cross_points->small.vector_field.set_coords(Coords(u_a, v_a));
        cross_points->big.vector_field.set_coords(Coords(u_b, v_b));

        return cross_points;
    } else {
        return std::nullopt;
    }
}

void second_interpolation(const CrossPoints &cross_points, Node &baseNode) {
    double p =
            Line(baseNode, cross_points.big).squareLength() / Line(cross_points.small, cross_points.big).squareLength();

    double u = p * cross_points.small.vector_field.coords.x + (1.0 - p) * cross_points.big.vector_field.coords.x;
    double v = p * cross_points.small.vector_field.coords.y + (1.0 - p) * cross_points.big.vector_field.coords.y;

    baseNode.vector_field.set_coords(Coords(u, v));
}

bool interpolation(const Element &element, Node &baseNode) {
    if (auto cross_points = first_interpolation(element, baseNode)) {
        second_interpolation(*cross_points, baseNode);
        return true;
    }
    return false;
}
