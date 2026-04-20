#pragma once

#include <memory>

#include "../SolverResult.h"
#include "ISolver.h"

namespace numerical {

class SolverFactory {
public:
    static std::unique_ptr<ISolver> create(MethodId methodId);
};

} // namespace numerical
