#include "FalsePositionSolver.h"

#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult FalsePositionSolver::solve(const SolverConfig &config) const {
    Validator::validateExpressionInput(config.functionExpression);
    Validator::validateInterval(config.intervalStart, config.intervalEnd);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);

    auto function = compileExpression(config.functionExpression);
    double a = config.intervalStart;
    double b = config.intervalEnd;
    double fa = function(a);
    double fb = function(b);

    if (fa * fb > 0.0) {
        throw MathError(
            "No sign change in [a, b]. The function may have no root or an even number of roots in this interval.");
    }

    SolverResult result;
    result.methodName = toDisplayName(MethodId::FalsePosition);
    result.headers = {"Iter", "a", "b", "xr", "f(a)", "f(b)", "f(xr)", "ea (%)"};
    result.functionSeries = sampleFunction(function, a, b);

    double previousXr = 0.0;
    bool hasPrevious = false;
    std::vector<std::string> lastRow;

    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const double denominator = fb - fa;
        if (std::abs(denominator) < 1e-15) {
            throw MathError("Division by zero encountered in False Position.");
        }

        const double xr = b - (fb * (a - b) / denominator);
        const double fxr = function(xr);
        const double error = hasPrevious
            ? relativeApproxErrorPercent(xr, previousXr)
            : std::numeric_limits<double>::quiet_NaN();

        lastRow = {
            std::to_string(iteration),
            formatNumber(a),
            formatNumber(b),
            formatNumber(xr),
            formatNumber(fa),
            formatNumber(fb),
            formatNumber(fxr),
            formatNumber(error),
        };
        if (config.showAllIterations) {
            result.rows.push_back(lastRow);
        }
        result.convergenceSeries.push_back({static_cast<double>(iteration), std::isfinite(error) ? error : 0.0});

        result.scalarResult = xr;
        result.iterations = iteration;
        result.achievedTolerance = std::isfinite(error) ? error / 100.0 : std::abs(fxr);
        result.markers = {{xr, fxr}};

        if (std::abs(fxr) <= config.tolerance ||
            (std::isfinite(error) && error <= config.tolerance * 100.0)) {
            result.converged = true;
            result.message = "Root estimated with regula falsi.";
            if (!config.showAllIterations) {
                result.rows = {lastRow};
            }
            return result;
        }

        if (fa * fxr < 0.0) {
            b = xr;
            fb = fxr;
        } else {
            a = xr;
            fa = fxr;
        }

        previousXr = xr;
        hasPrevious = true;
    }

    throw ConvergenceError("Did not converge in " +
                           std::to_string(config.maxIterations) +
                           " iterations. Try a smaller tolerance or a better interval.");
}

} // namespace numerical
