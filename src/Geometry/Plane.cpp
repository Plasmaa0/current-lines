#include "Plane.h"

Plane::Plane(const Node &nodeA, const Node &nodeB, const Node &nodeC) {
    auto &[x1, y1, z1] = nodeA.coords;
    auto &[x2, y2, z2] = nodeB.coords;
    auto &[x3, y3, z3] = nodeC.coords;
    // Ax + By + Cz + D = 0
    // Получим уравнение из определителя 3 порядка
    // | x-x1   y-y1   z-z1 |             | a0  b0  c0 |
    // | x2-x1 y2-y1  z2-z2 | =обозначим= | a1  b1  c1 | = 0
    // | x3-x1 y3-y1  z3-z1 |             | a2  b2  c2 |
    // Получаем, что:
    // a0*a + b0*b + c0*c = 0
    // (x-x1)*a + (y-y1)*b + (z-z1)*c = 0
    // xa-x1a + yb-y1b + zc-z1c = 0
    // xa + yb + zc + (- x1a - y1b - z1c) = 0
    // Следовательно d = - x1a - y1b - z1c
    auto a1 = x2 - x1;
    auto b1 = y2 - y1;
    auto c1 = z2 - z1;
    auto a2 = x3 - x1;
    auto b2 = y3 - y1;
    auto c2 = z3 - z1;
    A = b1 * c2 - b2 * c1; // алг. дополнение a0
    B = a2 * c1 - a1 * c2; // алг. дополнение a1
    C = a1 * b2 - b1 * a2; // алг. дополнение a2
    D = (-A * x1 - B * y1 - C * z1);
}

double Plane::findPosition(const Node &node) const {
    auto &[x, y, z] = node.coords;
    // Определить положение точки относительно плоскости
    return A * x + B * y + C * z + D;
}

std::ostream & operator<<(std::ostream &os, const Plane &obj) {
    return os << std::format("Plane({}x {}y {}z {}=0)", obj.A, obj.B, obj.C, obj.D);
}
