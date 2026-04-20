#include "NewtonRaphsonSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../../utils/NumericalDiff.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult NewtonRaphsonSolver::solve(const SolverConfig &config) const {
    Validator::validateExpressionInput(config.functionExpression);
    Validator::validateExpressionInput(config.derivativeExpression, true);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);
    Validator::validateFinite(config.initialGuess, "Initial guess");

    auto function = compileExpression(config.functionExpression);
    const bool hasExplicitDerivative = !config.derivativeExpression.empty();
    auto derivativeFunction = hasExplicitDerivative
        ? compileExpression(config.derivativeExpression)
        : std::function<double(double)> {};

    SolverResult result;
    result.methodName = toDisplayName(MethodId::NewtonRaphson);
    result.headers = {"Iter", "x_n", "f(x_n)", "f'(x_n)", "x_n+1", "ea (%)"};

    double x = config.initialGuess;
    std::vector<std::string> lastRow;

    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const double fx = function(x);
        const double dfx = hasExplicitDerivative
            ? derivativeFunction(x)
            : NumericalDiff::derivative(function, x);
        if (std::abs(dfx) < 1e-15) {
            throw MathError("Derivative is zero at x = " + formatNumber(x) +
                            ". Newton's method cannot continue.");
        }

        const double next = x - (fx / dfx);
        const double error = iteration == 1
            ? std::numeric_limits<double>::quiet_NaN()
            : relativeApproxErrorPercent(next, x);

        lastRow = {
            std::to_string(iteration),
            formatNumber(x),
            formatNumber(fx),
            formatNumber(dfx),
            formatNumber(next),
            formatNumber(error),
        };
        if (config.showAllIterations) {
            result.rows.push_back(lastRow);
        }
        result.convergenceSeries.push_back({static_cast<double>(iteration), std::isfinite(error) ? error : 0.0});

        result.scalarResult = next;
        result.iterations = iteration;
        result.achievedTolerance = std::isfinite(error) ? error / 100.0 : std::abs(fx);
        result.markers = {{next, function(next)}};

        if (std::abs(fx) <= config.tolerance ||
            (std::isfinite(error) && error <= config.tolerance * 100.0)) {
            result.converged = true;
            result.message = hasExplicitDerivative
                ? "Converged with user-provided derivative."
                : "Converged using numerical derivative fallback.";
            if (!config.showAllIterations) {
                result.rows = {lastRow};
            }
            const double minRange = std::min(config.initialGuess, next) - 2.0;
            const double maxRange = std::max(config.initialGuess, next) + 2.0;
            result.functionSeries = sampleFunction(function, minRange, maxRange);
            return result;
        }

        x = next;
    }

    throw ConvergenceError("Did not converge in " +
                           std::to_string(config.maxIterations) +
                           " iterations. Try a smaller tolerance or a better initial guess.");
}

} // namespace numerical
