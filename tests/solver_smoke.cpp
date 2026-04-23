#include <cmath>
#include <iostream>
#include <stdexcept>

#include "../core/solvers/BisectionSolver.h"
#include "../core/solvers/ConjugateGradientSolver.h"
#include "../core/solvers/FalsePositionSolver.h"
#include "../core/solvers/GaussianSolver.h"
#include "../core/solvers/GoldenSectionSolver.h"

namespace {

void expectNear(double actual, double expected, double tolerance, const char *label) {
    if (std::abs(actual - expected) > tolerance) {
        throw std::runtime_error(std::string(label) + " failed");
    }
}

} // namespace

int main() {
    using namespace numerical;

    BisectionSolver bisection;
    SolverConfig rootConfig;
    rootConfig.methodId = MethodId::Bisection;
    rootConfig.functionExpression = "x^3 - x - 2";
    rootConfig.intervalStart = 1.0;
    rootConfig.intervalEnd = 2.0;
    rootConfig.tolerance = 1e-6;
    rootConfig.maxIterations = 100;
    const auto rootResult = bisection.solve(rootConfig);
    expectNear(rootResult.scalarResult, 1.5213797, 1e-3, "Bisection");

    FalsePositionSolver falsePosition;
    SolverConfig falsePositionConfig;
    falsePositionConfig.methodId = MethodId::FalsePosition;
    falsePositionConfig.functionExpression = "x^2 - 3";
    falsePositionConfig.intervalStart = 1.0;
    falsePositionConfig.intervalEnd = 2.0;
    falsePositionConfig.tolerance = 1e-6;
    falsePositionConfig.maxIterations = 100;
    const auto falsePositionResult = falsePosition.solve(falsePositionConfig);
    expectNear(falsePositionResult.scalarResult, 1.7320508, 1e-3, "False Position");

    GaussianSolver gaussian;
    SolverConfig linearConfig;
    linearConfig.methodId = MethodId::GaussianElimination;
    linearConfig.matrixA = {
        {2.0, 1.0, -1.0},
        {-3.0, -1.0, 2.0},
        {-2.0, 1.0, 2.0},
    };
    linearConfig.vectorB = {8.0, -11.0, -3.0};
    const auto linearResult = gaussian.solve(linearConfig);
    expectNear(linearResult.vectorResult[0], 2.0, 1e-6, "Gaussian x");
    expectNear(linearResult.vectorResult[1], 3.0, 1e-6, "Gaussian y");
    expectNear(linearResult.vectorResult[2], -1.0, 1e-6, "Gaussian z");

    GoldenSectionSolver golden;
    SolverConfig optimizationConfig;
    optimizationConfig.methodId = MethodId::GoldenSection;
    optimizationConfig.functionExpression = "x^2 - 4*x + 4";
    optimizationConfig.intervalStart = 0.0;
    optimizationConfig.intervalEnd = 4.0;
    optimizationConfig.tolerance = 1e-5;
    optimizationConfig.maxIterations = 200;
    const auto optimizationResult = golden.solve(optimizationConfig);
    expectNear(optimizationResult.scalarResult, 2.0, 1e-3, "Golden Section");
    expectNear(optimizationResult.optimumValue, 0.0, 1e-3, "Golden Section f(x)");

    ConjugateGradientSolver cg;
    SolverConfig cgConfig;
    cgConfig.methodId = MethodId::ConjugateGradient;
    cgConfig.matrixA = {
        {4.0, 1.0},
        {1.0, 3.0},
    };
    cgConfig.vectorB = {1.0, 2.0};
    cgConfig.initialVector = {0.0, 0.0};
    cgConfig.tolerance = 1e-6;
    cgConfig.maxIterations = 25;
    const auto cgResult = cg.solve(cgConfig);
    expectNear(cgResult.vectorResult[0], 1.0 / 11.0, 1e-3, "Conjugate Gradient x");
    expectNear(cgResult.vectorResult[1], 7.0 / 11.0, 1e-3, "Conjugate Gradient y");

    std::cout << "solver_smoke: ok\n";
    return 0;
}
