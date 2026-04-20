#include "ConjugateGradientSolver.h"

#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult ConjugateGradientSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);
    Validator::validateTolerance(config.tolerance);
    Validator::validateMaxIterations(config.maxIterations);

    if (!isSymmetric(config.matrixA)) {
        throw ValidationError("Conjugate Gradient requires a symmetric matrix A.");
    }

    const int size = static_cast<int>(config.matrixA.size());
    std::vector<double> x = config.initialVector;
    if (x.empty()) {
        x.assign(static_cast<std::size_t>(size), 0.0);
    }
    if (static_cast<int>(x.size()) != size) {
        throw ValidationError("Initial guess vector size must match matrix size.");
    }

    std::vector<double> r = subtract(config.vectorB, multiply(config.matrixA, x));
    std::vector<double> p = r;
    double rsOld = dot(r, r);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::ConjugateGradient);
    result.headers = {"Iter", "x_n", "||r_n||", "p_n", "alpha_n"};

    if (std::sqrt(rsOld) <= config.tolerance) {
        result.converged = true;
        result.vectorResult = x;
        result.message = "Initial guess already satisfies the tolerance.";
        return result;
    }

    for (int iteration = 1; iteration <= config.maxIterations; ++iteration) {
        const auto ap = multiply(config.matrixA, p);
        const double denominator = dot(p, ap);
        if (denominator <= 0.0) {
            throw MathError("Matrix does not appear to be positive-definite for Conjugate Gradient.");
        }

        const double alpha = rsOld / denominator;
        x = add(x, scale(p, alpha));
        r = subtract(r, scale(ap, alpha));
        const double rsNew = dot(r, r);
        const double residualNorm = std::sqrt(rsNew);

        result.rows.push_back({
            std::to_string(iteration),
            formatVector(x),
            formatNumber(residualNorm),
            formatVector(p),
            formatNumber(alpha),
        });
        result.convergenceSeries.push_back({static_cast<double>(iteration), residualNorm});
        result.iterations = iteration;
        result.vectorResult = x;
        result.achievedTolerance = residualNorm;

        if (residualNorm <= config.tolerance) {
            result.converged = true;
            result.message = "Quadratic minimum reached with conjugate directions.";
            return result;
        }

        const double beta = rsNew / rsOld;
        p = add(r, scale(p, beta));
        rsOld = rsNew;
    }

    throw ConvergenceError("Did not converge in " +
                           std::to_string(config.maxIterations) +
                           " iterations. Try a better initial guess or verify the matrix is SPD.");
}

} // namespace numerical
