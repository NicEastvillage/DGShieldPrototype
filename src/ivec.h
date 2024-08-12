#ifndef DGSHIELDPROTOTYPE_IVEC_H
#define DGSHIELDPROTOTYPE_IVEC_H

#include <algorithm>
#include <functional>
#include "raylibns.h"

namespace DGShield {
    struct ivec {
        int x, y;

        ivec() : x(0), y(0) {};

        ivec(int px, int py) : x(px), y(py) {};

        ivec(rl::Vector2 v) : x(v.x), y(v.y) {};

        ivec operator+(const ivec &other) const {
            return {x + other.x, y + other.y};
        }

        ivec operator-(const ivec &other) const {
            return {x - other.x, y - other.y};
        }

        ivec operator*(int scalar) const {
            return {x * scalar, y * scalar};
        }

        [[nodiscard]] bool operator<(ivec v) const {
            return y < v.y || x < v.x;
        }

        [[nodiscard]] bool operator==(ivec v) const {
            return x == v.x && y == v.y;
        }

        [[nodiscard]] bool operator!=(ivec v) const {
            return !(*this == v);
        }

        [[nodiscard]] rl::Vector2 rl() const {
            return { (float)x, (float)y };
        }
    };

#define EMPTY_RECT {{0, 0}, {-1, -1}}

    struct irect {

        ivec min, max;

        irect(ivec corner1, ivec corner2) : min(std::min(corner1.x, corner2.x), std::min(corner1.y, corner2.y)),
                                            max(std::max(corner1.x, corner2.x), std::max(corner1.y, corner2.y)) {};

        [[nodiscard]] bool contains(ivec v) const {
            return min.x <= v.x && v.x <= max.x && min.y <= v.y && v.y <= max.y;
        }

        [[nodiscard]] bool isEmpty() const {
            return min.x > max.x || min.y > max.y;
        }

        [[nodiscard]] bool intersects(irect other) const {
            return isEmpty() || other.isEmpty() ||
                   (min.x <= other.max.x && other.min.x <= max.x && min.y <= other.max.y && other.min.y <= max.y);
        }

        [[nodiscard]] irect intersection(irect other) const {
            if (!intersects(other)) return EMPTY_RECT;
            return {
                    ivec(std::max(min.x, other.min.x), std::max(min.y, other.min.y)),
                    ivec(std::min(max.x, other.max.x), std::min(max.y, other.max.y)),
            };
        }

        [[nodiscard]] rl::Rectangle rl() const {
            if (isEmpty()) return { 0, 0, 0, 0 };
            return { (float)min.x, (float)min.y, (float)(max.x - min.x + 1), (float)(max.y - min.y + 1) };
        }

        [[nodiscard]] bool isSubsetOf(irect other) const {
            return isEmpty() || (!other.isEmpty() && other.min.x <= min.x && other.max.x >= max.x && other.min.y <= min.y && other.max.y >= max.y);
        }
    };
}

namespace std {
    template<>
    struct hash<DGShield::ivec> {
        const size_t operator()(const DGShield::ivec& v) const
        {
            return std::hash<int>()(v.x) ^ std::hash<int>()(v.y);
        }
    };
}

#endif //DGSHIELDPROTOTYPE_IVEC_H
