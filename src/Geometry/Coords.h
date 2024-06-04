#pragma once

struct Coords {
    Coords(double x_p, double y_p, double z_p=0);

    [[nodiscard]] bool operator==(const Coords &other) const;

    void set_coords(double x_p, double y_p, double z_p=0);

    [[nodiscard]] Coords get_normalize() const;

    double x;
    double y;
    double z;
};


