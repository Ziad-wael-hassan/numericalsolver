#pragma once

#include "ISolver.h"

namespace numerical {

class GaussJordanSolver final : public ISolver {
public:
    SolverResult solve(const SolverConfig &config) const override;
};

} // namespace numerical
