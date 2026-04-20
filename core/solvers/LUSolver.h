#pragma once

#include "ISolver.h"

namespace numerical {

class LUSolver final : public ISolver {
public:
    SolverResult solve(const SolverConfig &config) const override;
};

} // namespace numerical
