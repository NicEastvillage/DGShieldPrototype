#include <vector>
#include "ivec.h"

#ifndef DGSHIELDPROTOTYPE_MODEL_H
#define DGSHIELDPROTOTYPE_MODEL_H

namespace DGShield {
    enum tile_t {
        OK, DANGER
    };

    enum action_t {
        FORWARD, UP, DOWN
    };

    using state_t = ivec;

    struct partition_t {
        irect states;
        int level;
    };

    class Model {
    public:
        explicit Model(int pwidth, int pheight) : width(pwidth), height(pheight) {
            grid = new tile_t[width * height];
        };

        [[nodiscard]] tile_t at(ivec v) const {
            return grid[v.x + v.y * width];
        }

        [[nodiscard]] tile_t operator[](ivec v) const {
            return at(v);
        }

        void makeDangerous(irect r) {
            if (r.isEmpty()) return;
            for (int y = std::max(0, r.min.y); y <= std::min(height - 1, r.max.y); ++y) {
                for (int x = std::max(0, r.min.x); x <= std::min(width - 1, r.max.x); ++x) {
                    grid[x + y * width] = DANGER;
                }
            }
        }

        [[nodiscard]] state_t initial() const {
            return { 0, height / 2};
        }

        [[nodiscard]] std::vector<state_t> successors(state_t state, action_t action) const;

        [[nodiscard]] std::vector<state_t> successors(partition_t part, action_t action) const;

        void render() const;

    public:
        const int width;
        const int height;
    private:
        tile_t *grid;
    };
}


#endif //DGSHIELDPROTOTYPE_MODEL_H
