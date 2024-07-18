#ifndef DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
#define DGSHIELDPROTOTYPE_SHIELDGENERATOR_H

#include <cassert>
#include "Model.h"

namespace DGShield {
    enum assignment_t {
        UNEXPLORED, MAYBE, TRUE, FALSE
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

        [[nodiscard]] bool isCertain() const {
            assert(!isSplit());
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == MAYBE || action_allowed[i] == UNEXPLORED) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAllUncertain() const {
            assert(!isSplit());
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == TRUE || action_allowed[i] == FALSE) return false;
            }
            return true;
        }

        [[nodiscard]] bool hasAnyDisallowed() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == FALSE) return true;
            }
            return false;
        }

        [[nodiscard]] bool hasAnyUnexplored() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] == UNEXPLORED) return true;
            }
            return false;
        }

        [[nodiscard]] bool isAllAllowed() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] != TRUE) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAllDisallowed() const {
            for (int i = 0; i < std::size(action_allowed); ++i) {
                if (action_allowed[i] != FALSE) return false;
            }
            return true;
        }

        [[nodiscard]] const shield_node_t& findSmallestContaining(state_t state) const;
        [[nodiscard]] shield_node_t& findSmallestContaining(state_t state);

        void render(int height, bool rainbowShield) const;

    public:
        irect partition;
        int level;
        shield_node_t *lower{}, *upper{};
        assignment_t action_allowed[3] = { UNEXPLORED, UNEXPLORED, UNEXPLORED };
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

        [[nodiscard]] const shield_node_t& getShield() const {
            return _root;
        };

        void render(bool rainbowShield) const;

    private:
        shield_node_t findRoot(const Model &model);

        const shield_node_t& determineAssignmentsDFS(state_t state);

    private:
        bool _done = false;
        const Model& _model;
        shield_node_t _root;
    };
}

#endif //DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
