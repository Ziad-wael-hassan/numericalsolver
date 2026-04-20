#include "GoldenSectionSolver.h"

#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult GoldenSectionSolver::solve(const SolverConfig &config) const {
    Validator::validateExpressionInput(config.functionExpression);
    Validator::validateInterval(config.intervalStart, config.intervalEnd);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);

    auto function = compileExpression(config.functionExpression);
    const auto objective = [&function, maximize = config.maximize](double x) {
        const double value = function(x);
        return maximize ? -value : value;
    };

    double a = config.intervalStart;
    double b = config.intervalEnd;
    constexpr double ratio = 0.6180339887498948;
    double x1 = b - ratio * (b - a);
    double x2 = a + ratio * (b - a);
    double f1 = objective(x1);
    double f2 = objective(x2);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::GoldenSection);
    result.headers = {"Iter", "a", "b", "x1", "x2", "f(x1)", "f(x2)", "Interval"};
    result.functionSeries = sampleFunction(function, a, b);

    std::vector<std::string> lastRow;
    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const double intervalLength = std::abs(b - a);
        lastRow = {
            std::to_string(iteration),
            formatNumber(a),
            formatNumber(b),
            formatNumber(x1),
            formatNumber(x2),
            formatNumber(function(x1)),
            formatNumber(function(x2)),
            formatNumber(intervalLength),
        };
        if (config.showAllIterations) {
            result.rows.push_back(lastRow);
        }
        result.convergenceSeries.push_back({static_cast<double>(iteration), intervalLength});

        if (intervalLength <= config.tolerance) {
            const double optimumX = (a + b) / 2.0;
            result.scalarResult = optimumX;
            result.optimumValue = function(optimumX);
            result.iterations = iteration;
            result.achievedTolerance = intervalLength;
            result.markers = {{optimumX, result.optimumValue}};
            result.converged = true;
            result.message = config.maximize ? "Maximum bracketed successfully." : "Minimum bracketed successfully.";
            if (!config.showAllIterations) {
                result.rows = {lastRow};
            }
            return result;
        }

        if (f1 < f2) {
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = b - ratio * (b - a);
            f1 = objective(x1);
        } else {
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + ratio * (b - a);
            f2 = objective(x2);
        }
    }

    throw ConvergenceError("Did not converge in " +
                           std::to_string(config.maxIterations) +
                           " iterations. Try a wider interval or looser tolerance.");
}

} // namespace numerical
