#pragma once
#include <optional>
#include "Node.h"

class Line {
public:
    Node first_point;
    Node second_point;
    double coef_a, coef_b, coef_c;

    Line(const Node& first_point, const Node& second_point)
        : first_point(first_point),
          second_point(second_point),
          coef_a(first_point.coords.y - second_point.coords.y),
          coef_b(second_point.coords.x - first_point.coords.x),
          coef_c(first_point.coords.x * second_point.coords.y - second_point.coords.x * first_point.coords.y) {}

    bool the_same(const Line& other) const {
        return first_point.the_same(other.first_point) && second_point.the_same(other.second_point);
    }

    double find_position(const Node& node) const {
        return (node.coords.x - first_point.coords.x) * (second_point.coords.y - first_point.coords.y) -
               (node.coords.y - first_point.coords.y) * (second_point.coords.x - first_point.coords.x);
    }

    double len() const {
        return (second_point.coords.x - first_point.coords.x) * (second_point.coords.x - first_point.coords.x) +
               (second_point.coords.y - first_point.coords.y) * (second_point.coords.y - first_point.coords.y);
    }

    bool are_parall(const Line& other) const {
        return (coef_a == 0.0 && other.coef_a == 0.0) ||
               (coef_b == 0.0 && other.coef_b == 0.0) ||
               ((coef_a / other.coef_a) == (coef_b / other.coef_b) &&
                (coef_a != 0.0 && coef_b != 0.0 && other.coef_a != 0.0 && other.coef_b != 0.0));
    }

    std::optional<Node> lines_intersect(const Line& other) const {
        if (are_parall(other)) {
            return std::nullopt;
        }

        double det = coef_a * other.coef_b - other.coef_a * coef_b;
        double x = (coef_b * other.coef_c - other.coef_b * coef_c) / det;
        double y = (other.coef_a * coef_c - coef_a * other.coef_c) / det;

        return Node(-1, x, y);
    }
};


