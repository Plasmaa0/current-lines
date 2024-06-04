#pragma once

#include <Geometry/Node.h>
#include <Mesh/Mesh.h>

#include <vector>

class CurrentLineGenerator {
public:

    explicit CurrentLineGenerator(const Mesh &mesh_p);

    [[nodiscard]] std::vector<Node> generate_current_line(const Coords &baseCoords_p);

private:
    const Mesh &mesh;
    std::pair<double, double> size;
    double dx, dy;
};


