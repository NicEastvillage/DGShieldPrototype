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

    enum shield_render_mode_t {
        RMODE_LOCAL_SAFETY, RMODE_FUTURE_SAFETY, RMODE_RAINBOW
    };

    struct config_t;
    struct edge_t {
        action_t action;
        // Counter to indicate how many targets have unknown assignments, -1 if any target is unsafe
        int meta;
        config_t *source{};
        std::vector<state_t> targets{};
    };

    struct config_t {
    public:
        explicit config_t() : partition({0, 0}, {0, 0}), level(0) {}

        explicit config_t(irect partition, int level) : partition(partition), level(level) {
            assert(level >= 0);
        }

        ~config_t() {
            if (isSplit()) {
                delete[] children;
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

        // Split this configuration, but do not give child configurations any assignment or edges
        void fastSplit();

        // Split this configuration and transfer current assignment and dependants (incoming edges) to the children
        void heavySplit();

        [[nodiscard]] bool isSplit() const {
            return children != nullptr;
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

        [[nodiscard]] bool isAnySafe() const {
            for (int i = 0; i < std::size(assignment); ++i) {
                if (assignment[i] == SAFE) return true;
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

        // Returns true if configuration was reduced to a leaf partition where all actions are unsafe
        bool makeCompact();

        void render(int height, shield_render_mode_t rmode) const;

    public:
        irect partition;
        int level;
        assignment_t assignment[3] = {UNEXPLORED, UNEXPLORED, UNEXPLORED };
        // Order: lowlow, lowhigh, highlow, highhigh
        config_t *children = nullptr;
        edge_t dependencies[3]{};
        std::vector<edge_t*> dependants{};
    };

    class ShieldGeneratorDG {
    public:
        explicit ShieldGeneratorDG(const Model &model) : _model(model), _root(createRoot(model)) {}

        void reset() {
            if (_root.isSplit()) {
                delete[] _root.children;
            }
            _root = createRoot(_model);
            _forward_queue.clear();
            _back_queue.clear();
            _stat_back_props_total = 0;
            _stat_back_props_pruned = 0;
            _stat_explorations = 0;
            _stat_heavy_splits = 0;
            _done = false;
        }

        void run();

        [[nodiscard]] bool isDone() const { return _done; };

        [[nodiscard]] const config_t& getShield() const {
            return _root;
        };

        void makeCompact();

        void render(shield_render_mode_t rmode) const {
            _root.render(_model.height, rmode);
        };

    private:
        // Create a root partition that covers the entire state space
        static config_t createRoot(const Model &model);

        // Find the smallest configuration containing the given state and not containing any unsafe states (if possible).
        // Configurations may be split in order to find such a configuration.
        config_t& tryFindSafeConfigOf(state_t state);

        // Add and queue out-going edges of the given configuration.
        void explore(config_t& conf, bool exploreMaybes);

    private:
        bool _done = false;
        const Model& _model;
        config_t _root;
        std::vector<edge_t*> _forward_queue{};
        std::vector<edge_t*> _back_queue{};

        int _stat_back_props_total = 0;
        int _stat_back_props_pruned = 0;
        int _stat_explorations = 0;
        int _stat_heavy_splits = 0;
    };
}

#endif //DGSHIELDPROTOTYPE_SHIELDGENERATORDG_H
