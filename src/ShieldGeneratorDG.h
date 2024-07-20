#ifndef DGSHIELDPROTOTYPE_SHIELDGENERATORDG_H
#define DGSHIELDPROTOTYPE_SHIELDGENERATORDG_H

#include <memory>
#include <cassert>
#include "ivec.h"
#include "Model.h"

namespace DGShield {
    enum assignment_t {
        UNEXPLORED, MAYBE, SAFE, UNSAFE
    };

    struct config_t;
    struct edge_t {
        action_t action;
        int meta;
        config_t *source{};
        std::vector<state_t> targets{};
    };

    struct config_t {
    public:
        explicit config_t(irect partition, int level) : partition(partition), level(level) {
            assert(level >= 0);
        }

        ~config_t() {
            if (isSplit()) {
                delete child_low_low;
                delete child_low_high;
                delete child_high_low;
                delete child_high_high;
            }
        }

        void disallowAllActions() {
            std::fill(assignment, assignment + std::size(assignment), UNSAFE);
        }

        [[nodiscard]] int middleX() const {
            return partition.min.x + 1 + (partition.max.x - partition.min.x) / 2;
        }

        [[nodiscard]] int middleY() const {
            return partition.min.y + 1 + (partition.max.y - partition.min.y) / 2;
        }

        [[nodiscard]] bool canSplit() const {
            return !isSplit() && level > 0;
        }

        void fastSplit();

        void heavySplit();

        [[nodiscard]] bool isSplit() const {
            return child_low_low != nullptr;
        }

        [[nodiscard]] bool isAllCertain() const {
            assert(!isSplit());
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] == MAYBE || assignment[i] == UNEXPLORED) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAllUncertain() const {
            assert(!isSplit());
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] == SAFE || assignment[i] == UNSAFE) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAllSafe() const {
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] != SAFE) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAllUnsafe() const {
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] != UNSAFE) return false;
            }
            return true;
        }

        [[nodiscard]] bool isAnyUnsafe() const {
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] == UNSAFE) return true;
            }
            return false;
        }

        [[nodiscard]] bool isAnyUnexplored() const {
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] == UNEXPLORED) return true;
            }
            return false;
        }

        [[nodiscard]] const config_t& findSmallestContaining(state_t state) const;
        [[nodiscard]] config_t& findSmallestContaining(state_t state);

        void render(int height, bool rainbowShield) const;

    public:
        irect partition;
        int level;
        assignment_t assignment[3] = {UNEXPLORED, UNEXPLORED, UNEXPLORED };
        config_t *child_low_low{}, *child_low_high{}, *child_high_low{}, *child_high_high{};
        edge_t dependencies[3]{};
        std::vector<edge_t*> dependants{};
    };

    class ShieldGeneratorDG {
    public:
        explicit ShieldGeneratorDG(const Model &model) : _model(model), _root(findRoot(model)) {}

        void reset() {
            if (_root.isSplit()) {
                delete _root.child_low_low;
                delete _root.child_low_high;
                delete _root.child_high_low;
                delete _root.child_high_high;
            }
            _root = findRoot(_model);
            _done = false;
        }

        void run();

        [[nodiscard]] bool isDone() const { return _done; };

        [[nodiscard]] const config_t& getShield() const {
            return _root;
        };

        void render(bool rainbowShield) const {
            _root.render(_model.height, rainbowShield);
        };

    private:
        static config_t findRoot(const Model &model);

        config_t& findNontrivialConfig(state_t state);

        void findEdges(config_t& conf);

        void explore(config_t& conf, bool exploreMaybes);

    private:
        bool _done = false;
        const Model& _model;
        config_t _root;
        std::vector<edge_t*> _forward_queue{};
        std::vector<edge_t*> _back_queue{};
    };
}

#endif //DGSHIELDPROTOTYPE_SHIELDGENERATORDG_H
