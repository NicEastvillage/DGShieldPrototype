#ifndef DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
#define DGSHIELDPROTOTYPE_SHIELDGENERATOR_H

#include "Model.h"

namespace DGShield {
    class ShieldGenerator {
    public:
        explicit ShieldGenerator(Model &model) : model(model) {}

        void step();
        [[nodiscard]] bool isDone() const { return done; };
        [[nodiscard]] int getShield() const;
        void render() const;

    private:
        bool done = false;
        Model& model;
    };
}

#endif //DGSHIELDPROTOTYPE_SHIELDGENERATOR_H
