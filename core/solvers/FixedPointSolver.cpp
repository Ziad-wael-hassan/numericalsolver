#include "FixedPointSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../../utils/NumericalDiff.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult FixedPointSolver::solve(const SolverConfig &config) const {
    Validator::validateExpressionInput(config.fixedPointExpression);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);
    Validator::validateFinite(config.initialGuess, "Initial guess");

    auto g = compileExpression(config.fixedPointExpression);
    auto residual = [g](double x) {
        return g(x) - x;
    };

    const double derivativeAtStart = NumericalDiff::derivative(g, config.initialGuess);
    const bool convergenceCondition = std::abs(derivativeAtStart) < 1.0;

    SolverResult result;
    result.methodName = toDisplayName(MethodId::FixedPoint);
    result.headers = {"Iter", "x_n", "g(x_n)", "ea (%)"};
    result.functionSeries = sampleFunction(residual,
                                           config.initialGuess - 5.0,
                                           config.initialGuess + 5.0);

    double x = config.initialGuess;
    std::vector<std::string> lastRow;

    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const double next = g(x);
        if (!std::isfinite(next)) {
            throw MathError("Fixed-point iteration diverged to a non-finite value.");
        }

        const double error = iteration == 1
            ? std::numeric_limits<double>::quiet_NaN()
            : relativeApproxErrorPercent(next, x);

        lastRow = {
            std::to_string(iteration),
            formatNumber(x),
            formatNumber(next),
            formatNumber(error),
        };
        if (config.showAllIterations) {
            result.rows.push_back(lastRow);
        }
        result.convergenceSeries.push_back({static_cast<double>(iteration), std::isfinite(error) ? error : 0.0});

        result.scalarResult = next;
        result.iterations = iteration;
        result.achievedTolerance = std::isfinite(error) ? error / 100.0 : std::abs(next - x);
        result.markers = {{next, residual(next)}};

        if (std::abs(next - x) <= config.tolerance ||
            (std::isfinite(error) && error <= config.tolerance * 100.0)) {
            result.converged = true;
            result.message = convergenceCondition
                ? "Fixed-point iteration converged."
                : "Fixed-point iteration converged, but |g'(x0)| >= 1 so convergence was not guaranteed.";
            if (!config.showAllIterations) {
                result.rows = {lastRow};
            }
            return result;
        }

        x = next;
    }

    throw ConvergenceError(convergenceCondition
                               ? "Did not converge in the allotted iterations."
                               : "Convergence condition not met: |g'(x0)| = " +
                                     formatNumber(std::abs(derivativeAtStart)) +
                                     " >= 1. Method may diverge.");
}

} // namespace numerical
