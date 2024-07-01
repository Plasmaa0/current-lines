#pragma once

#include <ostream>
#include <Geometry/Node.h>

class Plane {
public:
    Plane(const Node &nodeA, const Node &nodeB, const Node &nodeC);

    [[nodiscard]] double findPosition(const Node &node) const;

    [[nodiscard]] Coords normal() const;

    friend std::ostream &operator<<(std::ostream &os, const Plane &obj);

private:
    double A, B, C, D; // Ax+By+Cz+D=0
};

DEFINE_FORMATTER(Plane)