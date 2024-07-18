#include "Trace.h"

void DGShield::trace_t::render() const {
    for (int i = 0; i < states.size(); i++) {
        state_t s = states[i];
        rl::Color color = model->containsDanger(s) ? rl::RED : rl::SKYBLUE;
        rl::DrawRectangle(s.x * rl::TILE_SIZE, (model->height - s.y - 1) * rl::TILE_SIZE, rl::TILE_SIZE, rl::TILE_SIZE, color);
        if (i > 0) {
            state_t p = states[i - 1];
            int half = (rl::TILE_SIZE + 1) / 2;
            rl::DrawLine(
                    p.x * rl::TILE_SIZE + half,
                    (model->height - p.y - 1) * rl::TILE_SIZE + half,
                    s.x * rl::TILE_SIZE + half,
                    (model->height - s.y - 1) * rl::TILE_SIZE + half,
                    color
            );
        }
    }
}
