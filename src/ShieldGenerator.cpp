#include <iostream>
#include "ShieldGenerator.h"

namespace DGShield {
    void shield_node_t::split() {
        assert(!isSplit());
        assert(level >= 0);
        int mid = middle();
        if (isSplitAxisX()) {
            lower = new shield_node_t({{partition.min.x, partition.min.y}, {mid - 1, partition.max.y}}, level - 1);
            upper = new shield_node_t({{mid, partition.min.y}, {partition.max.x, partition.max.y}}, level - 1);
        } else {
            lower = new shield_node_t({{partition.min.x, partition.min.y}, {partition.max.x, mid - 1}}, level - 1);
            upper = new shield_node_t({{partition.min.x, mid}, {partition.max.x, partition.max.y}}, level - 1);
        }
        std::copy(action_allowed, action_allowed + std::size(action_allowed), lower->action_allowed);
        std::copy(action_allowed, action_allowed + std::size(action_allowed), upper->action_allowed);
    }

    shield_node_t& shield_node_t::findSmallestContaining(state_t state) {
        assert(partition.contains(state));
        if (!isSplit()) return *this;
        if (isSplitAxisX()) {
            if (state.x < middle()) return lower->findSmallestContaining(state);
            else return upper->findSmallestContaining(state);
        } else {
            if (state.y < middle()) return lower->findSmallestContaining(state);
            else return upper->findSmallestContaining(state);
        }
    }

    void shield_node_t::render(int height) const {
        if (isSplit()) {
            lower->render(height);
            upper->render(height);
        } else {
            opt_bool res = hasStrategy();
            rl::Color color = rl::GOLD;
            if (res == TRUE) color = rl::GREEN;
            else if (res == FALSE) color = rl::RED;
            else if (hasAnyDisallowed()) color = rl::ORANGE;
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

    void ShieldGenerator::runToCompletion() {
        if (_done) return;
        std::cout << "RUNNING TO COMPLETION" << std::endl;
        determineStrategyDFS(_model.initial());
        _done = true;
    }

    opt_bool ShieldGenerator::determineStrategyDFS(state_t state) {
        /*
        INPUT: state
        OUTPUT: has strategy (ok, none, unknown)

        node = root.smallestNodeContaining(state);
        if node.on_stack:
            return UNKNOWN;
        while (true):
            if node.done:
                return node.strat;

            if node intersects BAD:
                if node.level == 0:
                    node.done = true;
                    node.strat = NONE;
                    return NONE;

                node.split()
                node = node.smallestNodeContaining(state);
                continue;
            break;

        // We found the smallest node not intersecting BAD
        node.on_stack = true;
        for action in node.unchecked_actions():
            // TODO: May visit same node many times to no prevail if cycle. Maybe cache unknown nodes if many cycles?
            for dest in succ(node.states, action):
                res = recurse(dest)
                if res == UNKNOWN:
                    // Cycle detected
                    continue action loop
                if res == NONE:
                    // It's possible to end up in BAD partition
                    if node.canSplit:
                        node.split();
                        node = node.smallestNodeContaining(state);
                        restart destination loop
                    else:
                        node.disallow(action);
                        continue action loop
                if res == OK:
                    // Destination has allowed action
                    noop
        if node.unchecked_actions() is empty:
            if node.has_allowed_actions():
                node.strat = OK;
            else:
                node.strat = NONE;
        node.on_stack = false;
        return node.strat;
        */

        shield_node_t *node = &_root.findSmallestContaining(state);
        if (node->on_stack) return MAYBE; // Cycle detected

        // Make sure smallest node does not intersect bad; split until true; return false if impossible
        while (true) {
            if (node->isDone()) return node->hasStrategy();
            if (_model.containsDanger(node->partition)) {
                if (node->level == 0) {
                    node->disallowAllActions();
                    return FALSE;
                }
                node->split();
                node = &node->findSmallestContaining(state);
                continue;
            }
            break;
        }

        // Determine all allowed actions of this node
        node->on_stack = true;
        for (int i = 0; i < std::size(node->action_allowed); ++i) {
            if (node->action_allowed[i] != MAYBE) continue;
            action_t action = (action_t)i;
            bool ok = true;
            for (state_t dest : _model.successors(node->partition, action)) { // TODO: Successor generator
                opt_bool hasStrategy = determineStrategyDFS(dest);
                if (hasStrategy == FALSE) {
                    // It is possible to end up in dangerous partition
                    if (node->canSplit()) {
                        // Split and try again with finer granularity
                        node->on_stack = false;
                        node->split();
                        node = &node->findSmallestContaining(state);
                        node->on_stack = true;
                        ok = false;
                        i = -1; // Restart action loop
                        break;
                    } else {
                        // Can't allow
                        node->action_allowed[i] = FALSE;
                        ok = false;
                        break;
                    }
                }
                if (hasStrategy == MAYBE) {
                    // It is possible to cycle
                    ok = false;
                }
            }
            if (ok) {
                node->action_allowed[i] = TRUE;
            }
        }
        node->on_stack = false;
        return node->hasStrategy();
    }

    void ShieldGenerator::step() {
        stackframe_t& frame = _stack.back();
        shield_node_t *node = &_root.findSmallestContaining(_model.initial());
        if (node->canSplit()) node->split();
    }

    int ShieldGenerator::getShield() const {
        return -1;
    }

    void ShieldGenerator::render() const {
        _root.render(_model.height);
    }

    shield_node_t ShieldGenerator::findRoot(const Model &model) {
        int size = 1;
        int topLevel = 0;
        while (size <= std::max(model.width, model.height)) {
            topLevel += 2;
            size *= 2;
        }
        return shield_node_t({{0, 0}, {size - 1, size - 1} }, topLevel);
    }
}
