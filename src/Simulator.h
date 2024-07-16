#ifndef DGSHIELDPROTOTYPE_SIMULATOR_H
#define DGSHIELDPROTOTYPE_SIMULATOR_H

#include "Model.h"
#include "Trace.h"

namespace DGShield {
    class Simulator {
    public:
        explicit Simulator(DGShield::Model &model) : model(model) {
            restart();
        }

        void restart();

        state_t step(action_t action);

        void finishWithRandomMoves();

        [[nodiscard]] trace_t getTrace() const { return trace; };

        void render() const;

    private:
        Model &model;
        trace_t trace;
    };
}

#endif //DGSHIELDPROTOTYPE_SIMULATOR_H
