#include "Line.h"

#include <complex>

Line::Line(const Node &A, const Node &B)
    : first_point(A),
      second_point(B),
      coef_a(A.coords.y - B.coords.y),
      coef_b(B.coords.x - A.coords.x),
      coef_c(A.coords.x * B.coords.y - B.coords.x * A.coords.y) {
}

bool Line::operator==(const Line &other) const {
    return first_point == other.first_point and second_point == other.second_point;
}

double Line::findPosition(const Node &node) const {
    // Предположим, у нас есть 3 точки: A(х1,у1), B(х2,у2), C(х3,у3).
    // Через точки A и Б проведена прямая.
    // И нам надо определить, как расположена точка С относительно прямой АБ.
    // Для этого вычисляем значение:
    //  D = (х3 - х1) * (у2 - у1) - (у3 - у1) * (х2 - х1)
    //      - Если D = 0 - значит, точка C лежит на прямой AB.
    //      - Если D < 0 - значит, точка C лежит слева от прямой.
    //      - Если D > 0 - значит, точка C лежит справа от прямой.
    return (node.coords.x - first_point.coords.x) * (second_point.coords.y - first_point.coords.y) -
           (node.coords.y - first_point.coords.y) * (second_point.coords.x - first_point.coords.x);
}

double Line::squareLength() const {
    double delta_x = second_point.coords.x - first_point.coords.x;
    double delta_y = second_point.coords.y - first_point.coords.y;
    double delta_z = second_point.coords.z - first_point.coords.z;
    return delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
}

double Line::length() const {
    return std::sqrt(squareLength());
}

bool Line::isParallel(const Line &other) const {
    return (coef_a == 0.0 && other.coef_a == 0.0) or
           (coef_b == 0.0 && other.coef_b == 0.0) or
           ((coef_a / other.coef_a) == (coef_b / other.coef_b) and
            (coef_a != 0.0 and coef_b != 0.0 and other.coef_a != 0.0 and other.coef_b != 0.0));
}

std::optional<Node> Line::linesIntersect(const Line &other) const {
    if (isParallel(other)) {
        return std::nullopt;
    }
    // СЛАУ
    //    a1 * x + b1 * y = -c1 первая прямая
    //    a2 * x + b2 * y = -c2 вторая прямая
    // Метод Крамера
    //    det = a1 * b2 - a2 * b1
    //    d_x = -c1 * b2 + b1 * c2
    //    d_x = -a1 * c2 + c1 * a2

    double det = coef_a * other.coef_b - other.coef_a * coef_b;
    double d_x = coef_b * other.coef_c - other.coef_b * coef_c;
    double d_y = other.coef_a * coef_c - coef_a * other.coef_c;
    double x = d_x / det;
    double y = d_y / det;

    return Node(Node::INVALID_ID, x, y);
}


std::ostream &operator<<(std::ostream &os, const Line &obj) {
    return os << std::format("Line(from {}, to {})", obj.first_point, obj.second_point);
}
