#pragma once
#include <Geometry/Coords.h>

inline int sign(auto x) {
    return x > 0 ? 1 : -1;
}

// Проверка, что v4 и p находятся по одну сторону от плоскости, построенной по трём точкам v1, v2, v3
inline bool SameSide(const Coords &v1, const Coords &v2, const Coords &v3, const Coords &v4, const Coords &p) {
    auto normal = (v2 - v1).cross(v3 - v1);
    auto dotV4 = normal.dot(v4 - v1);
    auto dotP = normal.dot(p - v1);
    // return dotV4 * dotP > 0;
    return sign(dotV4) == sign(dotP);
}
