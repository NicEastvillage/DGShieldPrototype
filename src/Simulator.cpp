#include "Simulator.h"

namespace DGShield {
    void Simulator::restart() {
        trace = trace_t({ model.initial() }, &model);
    }

    state_t Simulator::step(DGShield::action_t action) {
        state_t current = trace.states[trace.states.size() - 1];
        std::vector<state_t> destinations = model.successors(current, action);
        state_t dest = destinations[rand() % destinations.size()];
        trace.states.push_back(dest);
        return dest;
    }

    void Simulator::finishWithRandomMoves() {
        state_t current = trace.states[trace.states.size() - 1];
        auto limit = 10000;
        while (current != step(action_t(rand() % 3)) && limit-- > 0) {}
    }

    void Simulator::render() const {
        trace.render();
    }
}
