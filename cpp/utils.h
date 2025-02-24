#ifndef CPP_MODULE_UTILS_H
#define CPP_MODULE_UTILS_H

#include <string>

struct Position3D {
    double x;
    double y;
    double z;
};

struct Position2D {

    double x;
    double z;

    bool operator<(const Position2D& other) const {
        if (x != other.x)
            return x < other.x;
        return z < other.z;
    }

};

enum class Direction {
    XPositive,
    XNegative,
    ZPositive,
    ZNegative,
    UNKNOWN
};

#endif
