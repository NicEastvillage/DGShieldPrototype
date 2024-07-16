#include <set>
#include <cassert>
#include "Model.h"

namespace DGShield {
    std::vector<state_t> Model::successors(state_t state, action_t action) const {
        if (state.x == width - 1 || at(state) == DANGER) return { state };
        switch (action) {
            case FORWARD:
                // One or two steps right
                return { { state.x + 1, state.y }, { state.x + 2, state.y } };
            case UP:
                // One step right and up
                return { { state.x + 1, state.y + 1 } };
            case DOWN:
                // One step right and either one or two down
                return { { state.x + 1, state.y - 1 }, { state.x + 1, state.y - 2 } };
        }
        assert(false);
    }

    std::vector<state_t> Model::successors(partition_t part, action_t action) const {
        std::set<state_t> res;
        for (int y = std::max(0, part.states.min.y); y <= std::min(height - 1, part.states.max.y); ++y) {
            for (int x = std::max(0, part.states.min.x); x <= std::min(width - 1, part.states.max.x); ++x) {
                for (state_t s : successors({x, y}, action)) {
                    res.insert(s);
                }
            }
        }
        return { res.begin(), res.end() };
    }

    void Model::render() const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (at({x, y}) == DANGER) {
                    rl::DrawRectangle(x * rl::TILE_SIZE, (height - y - 1) * rl::TILE_SIZE, rl::TILE_SIZE, rl::TILE_SIZE, rl::GRAY);
                }
            }
        }
        state_t init = initial();
        rl::DrawRectangle(init.x * rl::TILE_SIZE, (height - init.y - 1) * rl::TILE_SIZE, rl::TILE_SIZE, rl::TILE_SIZE, rl::BLUE);
    }
}
