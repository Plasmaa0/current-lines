#include "CurrentLineGenerator.h"

#include <iostream>

#include "Utils/Logger.h"

CurrentLineGenerator::CurrentLineGenerator(const Mesh &mesh_p) : mesh(mesh_p), size(0, 0, 0) {
    auto &bBox = mesh.getBoundingBox();
    size = {bBox.size_x(), bBox.size_y(), bBox.size_z()};
    LOG_INFO("Initialized {}", *this);
    //    dx = bBox.size_x() / STEP_PRECISION;
    //    dy = bBox.size_y() / STEP_PRECISION;
    //    dz = bBox.size_z() / STEP_PRECISION;
}

CurrentLine CurrentLineGenerator::generate_current_line(const Coords &baseCoords_p) {
    LOG_INFO("Begin generating current line from {}", baseCoords_p);
    auto currentNode = Node(Node::INVALID_ID, baseCoords_p);
    auto currentElement = mesh.findElementByNode(currentNode);
    CurrentLine currentLine;
    if (not currentElement.has_value()) {
        LOG_ERROR("Base element not found");
        return currentLine;
    }
    LOG_INFO("Base element found {}, begin drawing", *currentElement->get());
    uint64_t linesCounter = 0;
    const auto linesCounterLimit = getLinesCounterLimit();
    while (linesCounter < linesCounterLimit) {
        // если в результате прошлой итерации мы вышли за границы КЭ
        if (not currentElement->get()->contains_node(currentNode)) {
            // то ищем новы КЭ в котором мы оказались
            currentElement = mesh.findElementByNode(currentNode);
        }

        // если новый КЭ не нашелся, то мы оказались за пределами модели
        if (not currentElement.has_value()) {
            LOG_INFO("Current line out of mesh bounds {}, current position {}", mesh.getBoundingBox(), currentNode.coords);
            break;
        }

        // если новый КЭ не нашелся, то интерполируем векторное поле КЭ в текущей точке
        auto c = currentElement->get()->interpolate(currentNode);
        currentNode.vector_field.set_coords(c);
        currentLine.appendNode(currentNode);

        currentNode.coords += currentNode.vector_field.coords_normalize * getElementStepSize(currentElement.value());

        linesCounter += 1;
    }
    LOG_INFO("Finished drawing current line. {}", currentLine);
    return currentLine;
}

uint64_t CurrentLineGenerator::getLinesCounterLimit() const {
    auto temp = size * POINTS_AMOUT_MULTIPLIER;
    return static_cast<uint64_t>(temp.x + temp.y + temp.z);
}

Coords CurrentLineGenerator::getElementStepSize(const std::shared_ptr<FE::Element> &elem) const {
    auto &bBox = elem->getBoundingBox();
    Coords res = {bBox.size_x(), bBox.size_y(), bBox.size_z()};
    return res / STEP_PRECISION;
}

std::vector<CurrentLine> CurrentLineGenerator::generate_current_lines(const Line &lineSegment_p, uint linesCount) {
    LOG_INFO("Generating {} current lines aligned by line from {} to {}", linesCount, lineSegment_p.first_point.coords,
             lineSegment_p.second_point.coords);
    auto step = (lineSegment_p.second_point.coords - lineSegment_p.first_point.coords) / linesCount;
    std::vector<Coords> basePoints;
    std::generate_n(std::back_inserter(basePoints), linesCount, [n = 0, lineSegment_p, step]() mutable {
        return lineSegment_p.first_point.coords + step * (n++);
    });
    LOG_INFO("Calculated {} base points, begin drawing", linesCount);
    std::vector<CurrentLine> currentLines;
    std::transform(basePoints.cbegin(), basePoints.cend(), std::back_inserter(currentLines),
                   [this](const Coords &basePoint) {
                       return generate_current_line(basePoint);
                   });
    LOG_INFO("Finished drawing {} current lines", linesCount);
    return currentLines;
}

std::ostream &operator<<(std::ostream &os, const CurrentLineGenerator &obj) {
    return os << std::format("CurrentLineGenerator({}, Points={}, Precision={})", obj.mesh,
                             obj.POINTS_AMOUT_MULTIPLIER, obj.STEP_PRECISION);
}
