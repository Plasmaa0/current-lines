#include "CrossPoints.h"

CrossPoints::CrossPoints(const Node &node_1, const Node &node_2) : small(node_1), big(node_2) {}

bool CrossPoints::operator==(const CrossPoints &other) const {
    return small == other.small and big == other.big;
}
