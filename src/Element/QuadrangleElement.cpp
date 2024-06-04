#include <numeric>
#include <algorithm>
#include "Geometry/Line.h"
#include "Element.h"
#include "QuadrangleElement.h"

Coords QuadrangleElement::interpolate(const Node &node) const {
    if (auto cross_points = find_cross_point_x(node)) {
        // первый шаг интерполяции
        double t = Line(getNodeByIndex(3), cross_points->small).squareLength() /
                   Line(getNodeByIndex(0), getNodeByIndex(3)).squareLength();
        double tau = Line(getNodeByIndex(2), cross_points->big).squareLength() /
                     Line(getNodeByIndex(1), getNodeByIndex(2)).squareLength();

        double u_a = t * getNodeByIndex(0).vector_field.coords.x + (1.0 - t) * getNodeByIndex(3).vector_field.coords.x;
        double v_a = t * getNodeByIndex(0).vector_field.coords.y + (1.0 - t) * getNodeByIndex(3).vector_field.coords.y;

        double u_b =
                tau * getNodeByIndex(1).vector_field.coords.x + (1.0 - tau) * getNodeByIndex(2).vector_field.coords.x;
        double v_b =
                tau * getNodeByIndex(1).vector_field.coords.y + (1.0 - tau) * getNodeByIndex(2).vector_field.coords.y;

        cross_points->small.vector_field.set_coords(Coords(u_a, v_a));
        cross_points->big.vector_field.set_coords(Coords(u_b, v_b));

        // второй шаг интерполяции
        double p = Line(node, cross_points->big).squareLength() /
                   Line(cross_points->small, cross_points->big).squareLength();

        double u = p * cross_points->small.vector_field.coords.x + (1.0 - p) * cross_points->big.vector_field.coords.x;
        double v = p * cross_points->small.vector_field.coords.y + (1.0 - p) * cross_points->big.vector_field.coords.y;
        return {u, v};
    } else {
        throw std::logic_error("interpolate error");
    }
}

QuadrangleElement::QuadrangleElement(Node &node1, Node &node2, Node &node3, Node &node4, uint id_p,
                                     Element::Type type_p) : Element(id_p, type_p) {
    nodes = std::vector<std::reference_wrapper<Node>>{std::ref(node1),
                                                      std::ref(node2),
                                                      std::ref(node3),
                                                      std::ref(node4)};
    sort_nodes();
}

std::optional<CrossPoints> QuadrangleElement::find_cross_point_x(const Node &node) const {
    Line scan_line(node, Node(Node::INVALID_ID, node.coords.x + 1.0, node.coords.y));

    if (node.coords.x == find_smallest_x() || node.coords.x == find_biggest_x()) {
        return std::nullopt; // FIXME крайний случай обраотать
    }

    std::optional<Node> small = scan_line.linesIntersect(Line(nodes[3], nodes[0]));
    std::optional<Node> big = scan_line.linesIntersect(Line(nodes[1], nodes[2]));

    if (small.has_value() && big.has_value()) {
        return CrossPoints(small.value(), big.value());
    }

    return std::nullopt; // FIXME такое вообще бывает? проверить
}