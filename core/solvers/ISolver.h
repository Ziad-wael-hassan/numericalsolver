#pragma once

#include "../SolverResult.h"

namespace numerical {

class ISolver {
public:
    virtual ~ISolver() = default;
    virtual SolverResult solve(const SolverConfig &config) const = 0;
};

} // namespace numerical
