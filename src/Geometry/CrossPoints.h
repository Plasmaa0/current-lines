#pragma once

#include <ostream>
#include <Geometry/Node.h>

class CrossPoints {
public:
    Node small;
    Node big;

    CrossPoints(const Node &node_1, const Node &node_2);

    [[nodiscard]] bool operator==(const CrossPoints &other) const;

    friend std::ostream &operator<<(std::ostream &os, const CrossPoints &obj);
};

DEFINE_FORMATTER(CrossPoints)