#ifndef DGSHIELDPROTOTYPE_TRACE_H
#define DGSHIELDPROTOTYPE_TRACE_H

#include <vector>
#include "Model.h"

namespace DGShield {
    struct trace_t {
        std::vector<state_t> states;

        void render(const Model& model) const;
    };
}

#endif //DGSHIELDPROTOTYPE_TRACE_H
