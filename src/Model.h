#include <vector>
#include "ivec.h"

#ifndef DGSHIELDPROTOTYPE_MODEL_H
#define DGSHIELDPROTOTYPE_MODEL_H

namespace DGShield {
    enum action_t {
        FORWARD, UP, DOWN
    };

    using state_t = ivec;

    class Model {
    public:
        explicit Model(int pwidth, int pheight) : width(pwidth), height(pheight), _initial( 0, height / 2), _dangers() {};

        [[nodiscard]] bool containsDanger(ivec v) const {
            for (irect danger : _dangers) {
                if (danger.contains(v)) return true;
            }
            return false;
        }

        [[nodiscard]] bool containsDanger(irect r) const {
            for (irect danger : _dangers) {
                if (danger.intersects(r)) return true;
            }
            return false;
        }

        void addDanger(irect r) {
            if (r.isEmpty()) return;
            _dangers.push_back(r);
        }

        void setInitial(state_t state) {
            _initial = state;
        }

        [[nodiscard]] state_t initial() const {
            return _initial;
        }

        [[nodiscard]] std::vector<state_t> successors(state_t state, action_t action) const;

        [[nodiscard]] std::vector<state_t> successors(irect states, action_t action) const;

        void render() const;

    public:
        const int width;
        const int height;
    private:
        state_t _initial;
        std::vector<irect> _dangers;
    };
}


#endif //DGSHIELDPROTOTYPE_MODEL_H
