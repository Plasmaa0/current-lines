#pragma once

#include <Geometry/Node.h>
#include <vector>
#include <fstream>

class CurrentLine {
public:
    void appendNode(const Node &node);

    void appendToFile(std::ofstream &file, uint offset = 0) const;

    [[nodiscard]] size_t size() const { return points.size(); }

private:
    std::vector<Node> points;

    static uint lineIDIncremental;

    void updateColors() const;
};
