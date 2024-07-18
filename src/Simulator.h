#ifndef DGSHIELDPROTOTYPE_SIMULATOR_H
#define DGSHIELDPROTOTYPE_SIMULATOR_H

#include "Model.h"
#include "Trace.h"

namespace DGShield {
    class Simulator {
    public:
        explicit Simulator(DGShield::Model &model) : _model(model) {
            restart();
        }

        void restart();

        state_t step(action_t action);

        void finishWithRandomMoves();

        [[nodiscard]] trace_t getTrace() const { return _trace; };

        void render() const;

    private:
        Model &_model;
        trace_t _trace;
    };
}

#endif //DGSHIELDPROTOTYPE_SIMULATOR_H
