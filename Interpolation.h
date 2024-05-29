#pragma once
#include <optional>
#include "Cell.h"
#include "Line.h"
#include "Node.h"

std::optional<CrossPoints> first_interpolation(const Cell& cell, Node& baseNode) {
    if (auto cross_points = cell.find_cross_point_x(baseNode)) {
        double t = Line(cell.nodes[3], cross_points->small).len() / Line(cell.nodes[0], cell.nodes[3]).len();
        double tau = Line(cell.nodes[2], cross_points->big).len() / Line(cell.nodes[1], cell.nodes[2]).len();

        double u_a = t * cell.nodes[0].vector_field.coords.x + (1.0 - t) * cell.nodes[3].vector_field.coords.x;
        double v_a = t * cell.nodes[0].vector_field.coords.y + (1.0 - t) * cell.nodes[3].vector_field.coords.y;

        double u_b = tau * cell.nodes[1].vector_field.coords.x + (1.0 - tau) * cell.nodes[2].vector_field.coords.x;
        double v_b = tau * cell.nodes[1].vector_field.coords.y + (1.0 - tau) * cell.nodes[2].vector_field.coords.y;

        cross_points->small.vector_field.set_coords(Coords(u_a, v_a));
        cross_points->big.vector_field.set_coords(Coords(u_b, v_b));

        return cross_points;
    } else {
        return std::nullopt;
    }
}

void second_interpolation(const CrossPoints& cross_points, Node& baseNode) {
    double p = Line(baseNode, cross_points.big).len() / Line(cross_points.small, cross_points.big).len();

    double u = p * cross_points.small.vector_field.coords.x + (1.0 - p) * cross_points.big.vector_field.coords.x;
    double v = p * cross_points.small.vector_field.coords.y + (1.0 - p) * cross_points.big.vector_field.coords.y;

    baseNode.vector_field.set_coords(Coords(u, v));
}

bool interpolation(const Cell& cell, Node& baseNode) {
    if (auto cross_points = first_interpolation(cell, baseNode)) {
        second_interpolation(*cross_points, baseNode);
        return true;
    }
    return false;
}

