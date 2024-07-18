#ifndef DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
#define DGSHIELDPROTOTYPE_SHIELDGENERATOR_H

#include <cassert>
#include "Model.h"

namespace DGShield {
    enum opt_bool {
        MAYBE, TRUE, FALSE
    };

    struct shield_node_t {
    public:
        explicit shield_node_t(irect partition, int level) : partition(std::move(partition)), level(level) {
            assert(level >= 0);
        }

        ~shield_node_t() {
            if (isSplit()) {
                delete lower;
                delete upper;
            }
        }

        void disallowAllActions() {
            std::fill(action_allowed, action_allowed + std::size(action_allowed), FALSE);
        }

        [[nodiscard]] int middle() const {
            if (isSplitAxisX()) return partition.min.x + 1 + (partition.max.x - partition.min.x) / 2;
            else return partition.min.y + 1 + (partition.max.y - partition.min.y) / 2;
        }

        [[nodiscard]] bool isSplitAxisX() const {
            return level % 2 == 0;
        }

        [[nodiscard]] bool canSplit() const {
            return !isSplit() && level > 0;
        }

        void split();

        [[nodiscard]] bool isSplit() const {
            return lower != nullptr && upper != nullptr;
        }

        [[nodiscard]] bool isDone() const {
            assert(!isSplit());
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == MAYBE) return false;
            }
            return true;
        }

        [[nodiscard]] opt_bool hasStrategy() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == MAYBE) return MAYBE;
                if (action_allowed[i] == TRUE) return TRUE;
            }
            return FALSE;
        }

        [[nodiscard]] bool hasAnyDisallowed() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == FALSE) return true;
            }
            return false;
        }

        [[nodiscard]] shield_node_t& findSmallestContaining(state_t state);

        void render(int height) const;

    public:
        irect partition;
        int level;
        shield_node_t *lower{}, *upper{};
        opt_bool action_allowed[3] = { MAYBE, MAYBE, MAYBE };
        bool on_stack = false;
    };

    class ShieldGenerator {
    public:
        explicit ShieldGenerator(const Model &model) : _model(model), _root(findRoot(model)) {}

        void reset() {
            if (_root.isSplit()) {
                delete _root.lower;
                delete _root.upper;
            }
            _root = findRoot(_model);
            _done = false;
        }

        void runToCompletion();

        void step();

        [[nodiscard]] bool isDone() const { return _done; };

        [[nodiscard]] int getShield() const;

        void render() const;

    private:
        enum status_t {
            INIT, ACTIONS
        };

        struct stackframe_t {
            shield_node_t &node;
            status_t status;
            action_t current_action;
        };

        shield_node_t findRoot(const Model &model);

        opt_bool determineStrategyDFS(state_t state);

    private:
        bool _done = false;
        const Model& _model;
        shield_node_t _root;
        std::vector<stackframe_t> _stack;
    };
}

#endif //DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
