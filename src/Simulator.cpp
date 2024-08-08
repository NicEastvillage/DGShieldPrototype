#include "Simulator.h"

namespace DGShield {
    void Simulator::reset() {
        _trace = trace_t({_model.initial() });
    }

    state_t Simulator::step(DGShield::action_t action) {
        state_t current = _trace.states[_trace.states.size() - 1];
        std::vector<state_t> destinations;
        for (state_t s : _model.successors(current, action)) {
            destinations.push_back(s);
        }
        state_t dest = destinations[rand() % destinations.size()];
        _trace.states.push_back(dest);
        return dest;
    }

    state_t Simulator::stepRandom() {
        return step(action_t(rand() % 3));
    }

    state_t Simulator::safeStepRandom() {
        if (!_shield_gen.isDone()) return stepRandom();
        state_t current = _trace.states[_trace.states.size() - 1];
        auto shield = _shield_gen.getShield().findSmallestContaining(current);
        if (shield.isAnyUnexplored() || shield.isAllUnsafe()) return stepRandom();
        action_t action = action_t(rand() % 3);
        while (shield.assignment[action] == UNSAFE) action = action_t(rand() % 3);
        std::vector<state_t> destinations;
        for (state_t s : _model.successors(current, action)) {
            destinations.push_back(s);
        }
        state_t dest = destinations[rand() % destinations.size()];
        _trace.states.push_back(dest);
        return dest;
    }

    void Simulator::finishWithRandomMoves() {
        state_t current = _trace.states[_trace.states.size() - 1];
        auto limit = 20000;
        while (current != safeStepRandom() && limit-- > 0) {}
    }

    void Simulator::render() const {
        _trace.render(_model);
    }
}
