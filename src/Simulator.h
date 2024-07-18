#ifndef DGSHIELDPROTOTYPE_SIMULATOR_H
#define DGSHIELDPROTOTYPE_SIMULATOR_H

#include "Model.h"
#include "Trace.h"
#include "ShieldGenerator.h"

namespace DGShield {
    class Simulator {
    public:
        explicit Simulator(const DGShield::Model &model, const ShieldGenerator& shield_gen) : _model(model), _shield_gen(shield_gen) {
            reset();
        }

        void reset();

        state_t step(action_t action);

        state_t stepRandom();

        state_t safeStepRandom();

        void finishWithRandomMoves();

        [[nodiscard]] trace_t getTrace() const { return _trace; };

        void render() const;

    private:
        const Model &_model;
        trace_t _trace;
        const ShieldGenerator& _shield_gen;
    };
}

#endif //DGSHIELDPROTOTYPE_SIMULATOR_H
