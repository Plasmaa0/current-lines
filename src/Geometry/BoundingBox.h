#pragma once

#include <tuple>

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

    [[nodiscard]] inline double size_z() const { return z_max() - z_min(); }
};