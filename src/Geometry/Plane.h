#pragma once

#include <Geometry/Node.h>

class Plane {
public:
    Plane(const Node &nodeA, const Node &nodeB, const Node &nodeC);

    [[nodiscard]] double findPosition(const Node &node) const;

private:
    double A, B, C, D; // Ax+By+Cz+D=0
};
