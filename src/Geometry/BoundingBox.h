#pragma once

#include <ostream>
#include <tuple>

#include "Coords.h"
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

    [[nodiscard]] inline double size_z() const { return z_max() - z_min(); }

    friend std::ostream &operator<<(std::ostream &os, const BoundingBox &obj) {
        return os << std::format("BoundingBox(x: [{}, {}], y: [{}, {}], z: [{}, {}])",
                                 obj.x_min(), obj.x_max(),
                                 obj.y_min(), obj.y_max(),
                                 obj.z_min(), obj.z_max());
    }

    BoundingBox operator+(const BoundingBox &other) const {
        return BoundingBox{
            std::pair(std::min(x_min(), other.x_min()), std::max(x_max(), other.x_max())),
            std::pair(std::min(y_min(), other.y_min()), std::max(y_max(), other.y_max())),
            std::pair(std::min(z_min(), other.z_min()), std::max(z_max(), other.z_max())),
        };
    }

    BoundingBox &operator+=(const BoundingBox &other) {
        return *this = *this + other;
    }

    bool contains(const Coords &point) const {
        return x_min() <= point.x && point.x <= x_max() &&
               y_min() <= point.y && point.y <= y_max() &&
               z_min() <= point.z && point.z <= z_max();
    }
};

DEFINE_FORMATTER(BoundingBox);
