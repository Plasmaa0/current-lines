#pragma once

#include <ostream>
#include <set>
#include <Geometry/Geometry.h>
#include <Mesh/Mesh.h>
#include <CurrentLineGenerator/CurrentLine.h>
#include <Element/Common/Element.h>

#include <vector>


class CurrentLineGenerator {
public:
    explicit CurrentLineGenerator(const Mesh &mesh_p);

    [[nodiscard]] CurrentLine generate_current_line(const Coords &baseCoords_p) const;

    [[nodiscard]] CurrentLine generate_current_line(
        const Node &baseNode_p,
        const std::optional<std::shared_ptr<FE::Element>> &baseElement_p = std::nullopt) const;

    [[nodiscard]] std::vector<CurrentLine> generate_current_lines(const Line &lineSegment_p, uint linesCount);

    [[nodiscard]] std::vector<CurrentLine> generate_current_lines(std::set<uint32_t> nodeIds_p) const;

private:
    const uint64_t STEP_PRECISION = 2;
    const uint64_t POINTS_AMOUT_MULTIPLIER = 50;
    const Mesh &mesh;
    Coords size;
    //    double dx, dy, dz;

    [[nodiscard]] uint64_t getLinesCounterLimit() const;

    [[nodiscard]] inline Coords getElementStepSize(const std::shared_ptr<FE::Element> &elem) const;

    friend std::ostream &operator<<(std::ostream &os, const CurrentLineGenerator &obj);
};

DEFINE_FORMATTER(CurrentLineGenerator)
