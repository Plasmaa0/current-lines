#pragma once

#include <ostream>
#include <tuple>

#include "Utils/Formatter.h"

struct BoundingBox {
    std::pair<double, double> xRange; // min-max
    std::pair<double, double> yRange; // min-max
    std::pair<double, double> zRange; // min-max
    [[nodiscard]] inline double x_min() const { return std::get<0>(xRange); }

    [[nodiscard]] inline double x_max() const { return std::get<1>(xRange); }

    [[nodiscard]] inline double y_min() const { return std::get<0>(yRange); }

    [[nodiscard]] inline double y_max() const { return std::get<1>(yRange); }

    [[nodiscard]] inline double z_min() const { return std::get<0>(zRange); }

    [[nodiscard]] inline double z_max() const { return std::get<1>(zRange); }

    [[nodiscard]] inline double size_x() const { return x_max() - x_min(); }

    [[nodiscard]] inline double size_y() const { return y_max() - y_min(); }

    friend std::ostream &operator<<(std::ostream &os, const BoundingBox &obj) {
        return os << std::format("BoundingBox(x: [{}, {}], y: [{}, {}], z: [{}, {}])",
                                 obj.x_min(), obj.x_max(),
                                 obj.y_min(), obj.y_max(),
                                 obj.z_min(), obj.z_max());
    }

    [[nodiscard]] inline double size_z() const { return z_max() - z_min(); }
};

DEFINE_FORMATTER(BoundingBox);