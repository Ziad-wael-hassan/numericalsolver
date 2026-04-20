#include "SolverFactory.h"

#include <memory>

#include "BisectionSolver.h"
#include "ConjugateGradientSolver.h"
#include "CramerSolver.h"
#include "FalsePositionSolver.h"
#include "FixedPointSolver.h"
#include "GaussJordanSolver.h"
#include "GaussianSolver.h"
#include "GoldenSectionSolver.h"
#include "LUSolver.h"
#include "NewtonRaphsonSolver.h"
#include "PALUSolver.h"
#include "SecantSolver.h"

namespace numerical {

std::unique_ptr<ISolver> SolverFactory::create(MethodId methodId) {
    switch (methodId) {
    case MethodId::Bisection:
        return std::make_unique<BisectionSolver>();
    case MethodId::FalsePosition:
        return std::make_unique<FalsePositionSolver>();
    case MethodId::NewtonRaphson:
        return std::make_unique<NewtonRaphsonSolver>();
    case MethodId::FixedPoint:
        return std::make_unique<FixedPointSolver>();
    case MethodId::Secant:
        return std::make_unique<SecantSolver>();
    case MethodId::GaussianElimination:
        return std::make_unique<GaussianSolver>();
    case MethodId::LUFactorization:
        return std::make_unique<LUSolver>();
    case MethodId::PALUFactorization:
        return std::make_unique<PALUSolver>();
    case MethodId::GaussJordan:
        return std::make_unique<GaussJordanSolver>();
    case MethodId::Cramer:
        return std::make_unique<CramerSolver>();
    case MethodId::GoldenSection:
        return std::make_unique<GoldenSectionSolver>();
    case MethodId::ConjugateGradient:
        return std::make_unique<ConjugateGradientSolver>();
    }
    return std::make_unique<BisectionSolver>();
}

} // namespace numerical
