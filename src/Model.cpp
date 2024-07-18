#include <set>
#include <cassert>
#include <unordered_set>
#include "Model.h"

namespace DGShield {
    std::vector<state_t> Model::successors(state_t state, action_t action) const {
        if (state.x == width - 1 || containsDanger(state)) return { state };
        switch (action) {
            case FORWARD:
                // One or two steps right
                if (state.x == width - 2) return { { state.x + 1, state.y } };
                return { { state.x + 1, state.y }, { state.x + 2, state.y } };
            case UP:
                // One step right and up
                if (state.y == height - 1) return { { state.x + 1, state.y } };
                return { { state.x + 1, state.y + 1 } };
            case DOWN:
                // One step right and either one or two down
                if (state.y == 0) return { { state.x + 1, state.y } };
                if (state.y == 1) return { { state.x + 1, state.y - 1 } };
                return { { state.x + 1, state.y - 1 }, { state.x + 1, state.y - 2 } };
        }
        assert(false);
    }

    std::vector<state_t> Model::successors(irect states, action_t action) const {
        assert(!states.isEmpty());
        std::unordered_set<state_t> res;
        if (action == UP) {
            for (int x = std::max(0, states.min.x); x <= std::min(width - 1, states.max.x); ++x) {
                for (state_t s : successors({x, std::min(height - 1, states.max.y)}, action)) {
                    if (!states.contains(s)) res.insert(s);
                }
            }
        }
        if (action == DOWN) {
            for (int x = std::max(0, states.min.x); x <= std::min(width - 1, states.max.x); ++x) {
                for (state_t s : successors({x, std::max(0, states.min.y)}, action)) {
                    if (!states.contains(s)) res.insert(s);
                }
            }
        }
        for (int y = std::max(0, states.min.y); y <= std::min(height - 1, states.max.y); ++y) {
            for (state_t s : successors({std::min(width - 1, states.max.x), y}, action)) {
                if (!states.contains(s)) res.insert(s);
            }
        }
        return { res.begin(), res.end() };
    }

    void Model::render() const {
        for (irect danger : _dangers) {
            rl::DrawRectangle(
                    danger.min.x * rl::TILE_SIZE,
                    (height - danger.max.y - 1) * rl::TILE_SIZE,
                    (danger.max.x - danger.min.x + 1) * rl::TILE_SIZE,
                    (danger.max.y - danger.min.y + 1) * rl::TILE_SIZE,
                    rl::GRAY
            );
        }
        state_t init = initial();
        rl::DrawRectangle(init.x * rl::TILE_SIZE, (height - init.y - 1) * rl::TILE_SIZE, rl::TILE_SIZE, rl::TILE_SIZE, rl::BLUE);
    }
}
