#pragma once

#include <algorithm>
#include <ostream>

class Color {
public:
    using ColorValue = unsigned char; // 0 to 255
    Color(ColorValue r, ColorValue g, ColorValue b) : r(r), g(g), b(b) {
    }

    void setColor(ColorValue r, ColorValue g, ColorValue b) {
        Color::r = std::clamp(r, static_cast<ColorValue>(0), static_cast<ColorValue>(255));
        Color::g = std::clamp(g, static_cast<ColorValue>(0), static_cast<ColorValue>(255));
        Color::b = std::clamp(b, static_cast<ColorValue>(0), static_cast<ColorValue>(255));
    }

    friend std::ostream &operator<<(std::ostream &os, const Color &obj) {
        return os << std::format("ColorRGB({}, {}, {})", obj.r, obj.g, obj.b);
    }

    bool operator==(const Color &other) const = default;

    ColorValue getR() const {
        return r;
    }

    void setR(ColorValue r) {
        Color::r = r;
    }

    ColorValue getG() const {
        return g;
    }

    void setG(ColorValue g) {
        Color::g = g;
    }

    ColorValue getB() const {
        return b;
    }

    void setB(ColorValue b) {
        Color::b = b;
    }

private:
    ColorValue r;
    ColorValue g;
    ColorValue b;
};

DEFINE_FORMATTER(Color)