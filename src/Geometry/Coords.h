#pragma once
#include <ostream>
#include <Utils/Formatter.h>

struct Coords {
    Coords();

    Coords(double x_p, double y_p, double z_p = 0);

    Coords(const Coords &other);

    Coords(Coords &&other) noexcept;

    [[nodiscard]] bool operator==(const Coords &other) const;

    Coords &operator=(const Coords &other);

    Coords &operator=(Coords &&other) noexcept;

    Coords &operator+=(const Coords &rhs);

    friend Coords operator+(Coords lhs, const Coords &rhs);

    Coords &operator-=(const Coords &rhs);

    friend Coords operator-(Coords lhs, const Coords &rhs);

    // element-vise (NOT CROSS PRODUCT, NOT DOT PRODUCT)
    Coords &operator*=(const Coords &rhs);

    friend Coords operator*(Coords lhs, const Coords &rhs);

    [[nodiscard]] double dot(const Coords &rhs) const;

    [[nodiscard]] Coords cross(const Coords &rhs) const;

    template<class T>
    Coords &operator*=(const T &coefficient) {
        x *= coefficient;
        y *= coefficient;
        z *= coefficient;
        return *this;
    }

    template<class T>
    friend Coords operator*(Coords lhs, const T &coefficient) {
        lhs *= coefficient;
        return lhs;
    }

    template<class T>
    Coords &operator/=(const T &coefficient) {
        x /= coefficient;
        y /= coefficient;
        z /= coefficient;
        return *this;
    }

    template<class T>
    friend Coords operator/(Coords lhs, const T &coefficient) {
        lhs /= coefficient;
        return lhs;
    }

    void set_coords(double x_p, double y_p, double z_p = 0);

    [[nodiscard]] Coords get_normalize() const;

    [[nodiscard]] double squareLen() const;

    [[nodiscard]] double len() const;

    friend std::ostream &operator<<(std::ostream &os, const Coords &obj);

    double x;
    double y;
    double z;
};


// смешанное произведение <a, b, c> = (a, [b, c])
double ScalarTripleProduct(const Coords &a, const Coords &b, const Coords &c);

DEFINE_FORMATTER(Coords)
