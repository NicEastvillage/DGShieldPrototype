#include <chrono>
#include <iostream>
#include "ShieldGeneratorDG.h"

namespace DGShield {
    void config_t::fastSplit() {
        assert(!isSplit());
        assert(level > 0);
        int midX = middleX();
        int midY = middleY();
        child_low_low = new config_t({{partition.min.x, partition.min.y}, {midX - 1, midY - 1}}, level - 1);
        child_low_high = new config_t({{partition.min.x, midY}, {midX - 1, partition.max.y}}, level - 1);
        child_high_low = new config_t({{midX, partition.min.y}, {partition.max.x, midY - 1}}, level - 1);
        child_high_high = new config_t({{midX, midY}, {partition.max.x, partition.max.y}}, level - 1);
    }

    void config_t::heavySplit() {
        fastSplit();
        // Each dependant must be assigned to the correct child
        for (edge_t *e : dependants) {
            for (state_t t : e->targets) {
                if (partition.contains(t)) {
                    if (child_low_low->partition.contains(t)) child_low_low->dependants.push_back(e);
                    else if (child_low_high->partition.contains(t)) child_low_high->dependants.push_back(e);
                    else if (child_high_low->partition.contains(t)) child_high_low->dependants.push_back(e);
                    else child_high_high->dependants.push_back(e);
                }
            }
        }
        dependants.clear();

        // Transfer current assignment
        std::copy(assignment, assignment + std::size(assignment), child_low_low->assignment);
        std::copy(assignment, assignment + std::size(assignment), child_low_high->assignment);
        std::copy(assignment, assignment + std::size(assignment), child_high_low->assignment);
        std::copy(assignment, assignment + std::size(assignment), child_high_high->assignment);
        std::fill(assignment, assignment + std::size(assignment), UNEXPLORED);
    }

    const config_t &config_t::findSmallestContaining(state_t state) const {
        assert(partition.contains(state));
        if (!isSplit()) return *this;
        int midX = middleX();
        int midY = middleY();
        if (state.x < midX && state.y < midY) return child_low_low->findSmallestContaining(state);
        if (state.x < midX && state.y >= midY) return child_low_high->findSmallestContaining(state);
        if (state.x >= midX && state.y < midY) return child_high_low->findSmallestContaining(state);
        return child_high_high->findSmallestContaining(state);
    }

    config_t &config_t::findSmallestContaining(state_t state) {
        assert(partition.contains(state));
        if (!isSplit()) return *this;
        int midX = middleX();
        int midY = middleY();
        if (state.x < midX && state.y < midY) return child_low_low->findSmallestContaining(state);
        if (state.x < midX && state.y >= midY) return child_low_high->findSmallestContaining(state);
        if (state.x >= midX && state.y < midY) return child_high_low->findSmallestContaining(state);
        return child_high_high->findSmallestContaining(state);
    }

    bool config_t::makeCompact() {
        if (!isSplit()) return isAllUnsafe();

        bool reduce = true;
        reduce &= child_low_low->makeCompact();
        reduce &= child_low_high->makeCompact();
        reduce &= child_high_low->makeCompact();
        reduce &= child_high_high->makeCompact();

        if (!reduce) return false;

        delete child_low_low; child_low_low = nullptr;
        delete child_low_high; child_low_high = nullptr;
        delete child_high_low; child_high_low = nullptr;
        delete child_high_high; child_high_high = nullptr;

        std::fill(assignment, assignment + std::size(assignment), UNSAFE);
        return true;
    }

    void config_t::render(int height, shield_render_mode_t rmode) const {
        if (isSplit()) {
            child_low_low->render(height, rmode);
            child_low_high->render(height, rmode);
            child_high_low->render(height, rmode);
            child_high_high->render(height, rmode);
        } else {
            rl::Color color;
            switch (rmode) {
                case RMODE_LOCAL_SAFETY:
                    color = rl::GREEN;
                    if (isAnyUnexplored()) color = rl::GOLD;
                    else if (isAllUnsafe()) color = rl::RED;
                    break;
                case RMODE_FUTURE_SAFETY:
                    if (isAnyUnexplored()) return;
                    color = { 142, 201, 255, 255 };
                    if (isAllSafe()) color = rl::GREEN;
                    else if (isAllUnsafe()) color = rl::RED;
                    break;
                case RMODE_RAINBOW:
                    if (isAnyUnexplored()) return;
                    color = { 224, 224, 224, 255 };
                    if (assignment[0] == UNSAFE) color.r = 0;
                    if (assignment[1] == UNSAFE) color.g = 0;
                    if (assignment[2] == UNSAFE) color.b = 0;
                    break;
            }

            color.a = 15;
            rl::DrawRectangle(
                    partition.min.x * rl::TILE_SIZE,
                    (height - partition.max.y - 1) * rl::TILE_SIZE,
                    (partition.max.x - partition.min.x + 1) * rl::TILE_SIZE,
                    (partition.max.y - partition.min.y + 1) * rl::TILE_SIZE,
                    color
            );
            color.a = 255;
            rl::DrawLine(
                    partition.min.x * rl::TILE_SIZE,
                    (height - partition.max.y - 1) * rl::TILE_SIZE + 1,
                    (partition.max.x + 1) * rl::TILE_SIZE,
                    (height - partition.max.y - 1) * rl::TILE_SIZE + 1,
                    color
            );
            rl::DrawLine(
                    partition.min.x * rl::TILE_SIZE + 1,
                    (height - partition.max.y - 1) * rl::TILE_SIZE + 1,
                    partition.min.x * rl::TILE_SIZE + 1,
                    (height - partition.min.y) * rl::TILE_SIZE,
                    color
            );
        }
    }

    config_t ShieldGeneratorDG::createRoot(const Model &model) {
        int size = 1;
        int rootLevel = 0;
        while (size < std::max(model.width, model.height)) {
            rootLevel++;
            size *= 2;
        }
        return config_t({{0, 0}, {size - 1, size - 1} }, rootLevel);
    }

    void ShieldGeneratorDG::run() {
        if (_done) return;

        std::cout << "Running algorithm ... ";
        auto startTime = std::chrono::high_resolution_clock::now();

        config_t& init_conf = tryFindSafeConfigOf(_model.initial());
        explore(init_conf, false);

        while (!_back_queue.empty() || !_forward_queue.empty()) {

            if (!_back_queue.empty()) {
                _stat_back_props_total++;
                edge_t* edge = _back_queue.back();
                _back_queue.pop_back();

                assignment_t source_assign = edge->source->assignment[edge->action];
                if (source_assign == SAFE || source_assign == UNSAFE || edge->source->isSplit()) {
                    // Edge is outdated
                    _stat_back_props_pruned++;
                    continue;
                }

                if (edge->meta == -1) { // Some target is unsafe
                    if (edge->source->canSplit()) {
                        // Attempt to split for higher accuracy
                        edge->source->heavySplit();
                        // Construct edges of children
                        explore(*edge->source->child_low_low, true);
                        explore(*edge->source->child_low_high, true);
                        explore(*edge->source->child_high_low, true);
                        explore(*edge->source->child_high_high, true);
                        _stat_heavy_splits++;
                    } else {
                        // We cannot allow this action in the source configuration
                        edge->source->assignment[edge->action] = UNSAFE;
                        if (edge->source->isAllUnsafe()) {
                            // Queue back-propagation of danger
                            for (edge_t *e : edge->source->dependants) {
                                e->meta = -1;
                                _back_queue.push_back(e);
                            }
                        }
                    }
                } else if (edge->meta == 0) { // All targets are safe
                    edge->source->assignment[edge->action] = SAFE;
                    if (edge->source->isAllSafe()) {
                        for (edge_t *e : edge->source->dependants) {
                            if (e->meta > 0) {
                                e->meta -= 1;
                                if (e->meta == 0) {
                                    // Queue back-propagation of safety
                                    _back_queue.push_back(e);
                                }
                            }
                        }
                        edge->targets.clear();
                    }
                } else {
                    assert(false && "meta counter is always -1 or 0 for edges in _back_queue");
                }

            } else {
                _stat_explorations++;
                edge_t* edge = _forward_queue.back();
                _forward_queue.pop_back();

                assignment_t source_assign = edge->source->assignment[edge->action];
                if (source_assign == SAFE || source_assign == UNSAFE || edge->source->isSplit()) {
                    // Edge is outdated
                    continue;
                }

                assert(edge->targets.empty() && "edges in _forward_queue has no targets yet");

                // Compute targets of edge
                assert(edge->targets.empty());
                bool ok = true;
                for (state_t s : _model.successors(edge->source->partition, edge->action)) {
                    config_t& conf = tryFindSafeConfigOf(s);
                    if (conf.isAllUnsafe()) {
                        ok = false;
                        break;
                    }
                    if (conf.isAllSafe()) {
                        continue;
                    }
                    if (conf.isAnyUnexplored()) {
                        explore(conf, false);
                    }
                    edge->targets.push_back(s);
                    conf.dependants.push_back(edge);
                }
                if (!ok) {
                    // A target is completely unsafe; not ok; we can immediately back propagate
                    edge->targets.clear();
                    edge->meta = -1;
                    _back_queue.push_back(edge);
                } else {
                    edge->meta = edge->targets.size();
                    if (edge->meta == 0) {
                        // Edge has no targets and trivially safe; Queue for back-propagation immediately
                        _back_queue.push_back(edge);
                    }
                }
            }
        }
        _done = true;

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Done" << std::endl;
        std::cout << "- Time spent: " << duration << std::endl;
        std::cout << "- Back props (incl. pruned): " << _stat_back_props_total << std::endl;
        std::cout << "- Back props (excl. pruned): " << (_stat_back_props_total - _stat_back_props_pruned) << std::endl;
        std::cout << "- Explorations: " << _stat_explorations << std::endl;
        std::cout << "- Heavy splits: " << _stat_heavy_splits << std::endl;
    }

    config_t& ShieldGeneratorDG::tryFindSafeConfigOf(state_t state) {
        config_t* conf = &_root.findSmallestContaining(state);
        // Make sure smallest configuration does not intersect bad; split until true (if possible)
        while (true) {
            if (_model.containsDanger(conf->partition)) {
                if (conf->level == 0) {
                    conf->disallowAllActions();
                    return *conf;
                }
                conf->fastSplit();
                conf = &conf->findSmallestContaining(state);
                continue;
            }
            break;
        }
        return *conf;
    }

    void ShieldGeneratorDG::explore(config_t &conf, bool exploreMaybes) {
        for (int action = 0; action < 3; ++action) {
            if (conf.assignment[action] == UNEXPLORED || (exploreMaybes && conf.assignment[action] == MAYBE)) {
                conf.assignment[action] = MAYBE;
                conf.dependencies[action] = edge_t((action_t)action, INT_MAX, &conf, {});
                _forward_queue.push_back(&conf.dependencies[action]);
            }
        }
    }

    void ShieldGeneratorDG::makeCompact() {
        if (!isDone()) return;
        _root.makeCompact();
    }
}
