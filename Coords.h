#include <cmath>

class Coords {
public:
    Coords(double x_p, double y_p, double z_p=0) {
        x = x_p;
        y = y_p;
        z = z_p;
    }

    bool the_same(const Coords& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    void set_coords(double x_p, double y_p, double z_p=0) {
        x = x_p;
        y = y_p;
        z = z_p;
    }

    Coords get_normalize() const {
        double length = std::sqrt(x * x + y * y + z * z);
        return Coords(x / length, y / length, z / length);
    }

// private:
    double x;
    double y;
    double z;
};


