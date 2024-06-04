#pragma once
#include <Geometry/Node.h>

class CrossPoints {
public:
    Node small;
    Node big;

    CrossPoints(const Node& node_1, const Node& node_2);

    [[nodiscard]] bool the_same(const CrossPoints& other) const;
};

