#pragma once

#include <Geometry/Node.h>

#include <optional>

class Line {
    // https://ru.wikipedia.org/wiki/Прямая#Уравнение_прямой,_проходящей_через_две_точки_на_вещественной_плоскости
public:
    Node first_point;
    Node second_point;
    double coef_a, coef_b, coef_c;

    Line(const Node &A, const Node &B);

    [[nodiscard]] bool operator==(const Line &other) const;

    [[nodiscard]] double findPosition(const Node &node) const;

    [[nodiscard]] double squareLength() const;

    [[nodiscard]] double length() const;

    [[nodiscard]] bool isParallel(const Line &other) const;

    [[nodiscard]] std::optional<Node> linesIntersect(const Line &other) const;
};


