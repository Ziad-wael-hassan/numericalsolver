#include "SecantSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult SecantSolver::solve(const SolverConfig &config) const {
    Validator::validateExpressionInput(config.functionExpression);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);
    Validator::validateFinite(config.initialGuess, "Initial guess x0");
    Validator::validateFinite(config.secondInitialGuess, "Initial guess x1");

    auto function = compileExpression(config.functionExpression);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::Secant);
    result.headers = {"Iter", "x_n-1", "x_n", "x_n+1", "f(x_n)", "ea (%)"};

    double x0 = config.initialGuess;
    double x1 = config.secondInitialGuess;
    std::vector<std::string> lastRow;

    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const double f0 = function(x0);
        const double f1 = function(x1);
        const double denominator = f1 - f0;
        if (std::abs(denominator) < 1e-15) {
            throw MathError("Division by zero encountered during the Secant update.");
        }

        const double x2 = x1 - (f1 * (x1 - x0) / denominator);
        const double error = iteration == 1
            ? std::numeric_limits<double>::quiet_NaN()
            : relativeApproxErrorPercent(x2, x1);

        lastRow = {
            std::to_string(iteration),
            formatNumber(x0),
            formatNumber(x1),
            formatNumber(x2),
            formatNumber(f1),
            formatNumber(error),
        };
        if (config.showAllIterations) {
            result.rows.push_back(lastRow);
        }
        result.convergenceSeries.push_back({static_cast<double>(iteration), std::isfinite(error) ? error : 0.0});

        result.scalarResult = x2;
        result.iterations = iteration;
        result.achievedTolerance = std::isfinite(error) ? error / 100.0 : std::abs(function(x2));
        result.markers = {{x2, function(x2)}};

        if (std::abs(function(x2)) <= config.tolerance ||
            (std::isfinite(error) && error <= config.tolerance * 100.0)) {
            result.converged = true;
            result.message = "Converged without requiring an explicit derivative.";
            if (!config.showAllIterations) {
                result.rows = {lastRow};
            }
            const double start = std::min({config.initialGuess, config.secondInitialGuess, x2}) - 2.0;
            const double end = std::max({config.initialGuess, config.secondInitialGuess, x2}) + 2.0;
            result.functionSeries = sampleFunction(function, start, end);
            return result;
        }

        x0 = x1;
        x1 = x2;
    }

    throw ConvergenceError("Did not converge in " +
                           std::to_string(config.maxIterations) +
                           " iterations. Try different initial guesses.");
}

} // namespace numerical
