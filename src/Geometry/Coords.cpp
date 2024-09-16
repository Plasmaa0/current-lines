#include "Coords.h"
#include <cmath>

Coords::Coords(double x_p, double y_p, double z_p) {
    x = x_p;
    y = y_p;
    z = z_p;
}

bool Coords::operator==(const Coords &other) const {
    return x == other.x && y == other.y && z == other.z;
}

void Coords::set_coords(double x_p, double y_p, double z_p) {
    x = x_p;
    y = y_p;
    z = z_p;
}

Coords Coords::get_normalize() const {
    double length = len();
    return {x / length, y / length, z / length};
}

Coords::Coords() : Coords(0, 0, 0) {
}

Coords &Coords::operator+=(const Coords &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Coords operator+(Coords lhs, const Coords &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Coords &Coords::operator-=(const Coords &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Coords operator-(Coords lhs, const Coords &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Coords &Coords::operator*=(const Coords &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Coords operator*(Coords lhs, const Coords &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

std::ostream &operator<<(std::ostream &os, const Coords &obj) {
    return os << std::format("Coords({}, {}, {})", obj.x, obj.y, obj.z);
}

double Coords::dot(const Coords &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Coords Coords::cross(const Coords &rhs) const {
    return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x};
}

Coords &Coords::operator=(const Coords &other) {
    if (this == &other)
        return *this;
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Coords &Coords::operator=(Coords &&other) noexcept {
    // Guard self assignment
    if (this == &other)
        return *this; // delete[]/size=0 would also be ok
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Coords::Coords(const Coords &other) : x(other.x), y(other.y), z(other.z) {
}

Coords::Coords(Coords &&other) noexcept: x(other.x), y(other.y), z(other.z) {
}

double Coords::squareLen() const {
    return x * x + y * y + z * z;
}

double Coords::len() const {
    return std::sqrt(squareLen());
}

double ScalarTripleProduct(const Coords &a, const Coords &b, const Coords &c) {
    return a.dot(b.cross(c));
}
