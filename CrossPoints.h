#include "Node.h"

class CrossPoints {
public:
    Node small;
    Node big;

    CrossPoints(const Node& node_1, const Node& node_2) : small(node_1), big(node_2) {}

    bool the_same(const CrossPoints& other) const {
        return small.the_same(other.small) && big.the_same(other.big);
    }
};

