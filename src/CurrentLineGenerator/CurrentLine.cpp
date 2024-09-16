#include "CurrentLine.h"
#include <iostream>

#include "Utils/Logger.h"

uint CurrentLine::lineIDIncremental = 1;

void CurrentLine::appendNode(const Node &node) {
    points.push_back(node);
}

bool CurrentLine::appendToFile(std::ofstream &file, uint offset) const {
    if (not file.is_open()) {
        LOG_ERROR("File not open whine saving CurrentLine");
        throw std::invalid_argument("FILE NOT OPEN");
    }
    if (size() > 10000) return false;
    LOG_TRACE("Saving {} to file", *this);
    updateColors();
    //    Color {r,g,b}{Point(id) = {x, y, z, 1};}
    if (points.size() >= 2) {
        for (std::vector<Node>::size_type i = 0; i < points.size(); ++i) {
            const auto &node = points[i];
            file << "Color {"
                    << static_cast<int>(node.color.getR()) << ", "
                    << static_cast<int>(node.color.getG()) << ", "
                    << static_cast<int>(node.color.getB()) << "}{";
            file << "Point(" << i + 1 + offset << ") = {" << node.coords.x << ", " << node.coords.y << ", " << node.
                    coords.z
                    << ", "
                    << 1 << "};}" << std::endl;
        }
        file << "Line(" << CurrentLine::lineIDIncremental << ") = {";
        for (std::vector<Node>::size_type i = 0; i < points.size(); ++i) {
            file << i + 1 + offset;
            if (i != points.size() - 1)
                file << ", ";
        }
        file << "};" << std::endl << std::flush;
        ++CurrentLine::lineIDIncremental;
        return true;
    }
    return false;
}

// вычислить f(x), где f такая, что:
// f(x1)=f1,
// f(x2)=f2
double colorInterpolation(double x1, double f1, double x2, double f2, double x) {
    auto delta_y = f2 - f1;
    auto delta_x = x2 - x1;
    return delta_y / delta_x * (x - x1) + f1;
}

void CurrentLine::updateColors() const {
    auto maxLen = 0.0;
    auto minLen = std::numeric_limits<double>::max();
    for (auto &node: points) {
        auto len = node.vector_field.coords.len();
        if (len > maxLen) maxLen = len;
        if (len < minLen) minLen = len;
    }
    auto delta = maxLen - minLen;
    auto a = minLen;
    auto b = minLen + delta * 0.2;
    auto c = minLen + delta * 0.35;
    auto d = minLen + delta * 0.65;
    auto k = minLen + delta * 0.8;
    auto f = maxLen;

    for (auto &node: points) {
        auto len = node.vector_field.coords.len();
        if (len <= b) {
            //blue increase, g=0, r=0
            node.color.setColor(0, 0, colorInterpolation(a, 128, b, 255, len));
        } else if (b < len and len <= c) {
            //blue=1,r=0, green increase
            node.color.setColor(0, colorInterpolation(b, 0, c, 255, len), 255);
        } else if (c < len and len <= d) {
            //green=1, blue decrease, red increase
            node.color.setColor(colorInterpolation(c, 0, d, 255, len), 255, colorInterpolation(c, 255, d, 0, len));
        } else if (d < len and len <= k) {
            //red=1,b=0, green decrease
            node.color.setColor(255, colorInterpolation(d, 255, k, 0, len), 0);
        } else {
            // red decrease, g=0, b=0
            node.color.setColor(colorInterpolation(k, 255, f, 128, len), 0, 0);
        }
    }
}

std::ostream &operator<<(std::ostream &os, const CurrentLine &obj) {
    return os << std::format("CurrentLine(size={})", obj.points.size());
}
