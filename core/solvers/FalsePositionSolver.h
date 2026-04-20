#pragma once

#include "ISolver.h"

namespace numerical {

class FalsePositionSolver final : public ISolver {
public:
    SolverResult solve(const SolverConfig &config) const override;
};

} // namespace numerical
