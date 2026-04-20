#include "CramerSolver.h"

#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult CramerSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);

    const int size = static_cast<int>(config.matrixA.size());
    const double detA = determinant(config.matrixA);
    if (std::abs(detA) < 1e-12) {
        throw MathError("Matrix is singular (det = 0). The system has no unique solution.");
    }

    SolverResult result;
    result.methodName = toDisplayName(MethodId::Cramer);
    result.headers = {"Variable", "det(A_i)", "x_i"};
    result.vectorResult.resize(static_cast<std::size_t>(size));

    for (int column = 0; column < size; ++column) {
        auto replaced = config.matrixA;
        for (int row = 0; row < size; ++row) {
            replaced[row][column] = config.vectorB[row];
        }
        const double detAi = determinant(replaced);
        const double xi = detAi / detA;
        result.vectorResult[static_cast<std::size_t>(column)] = xi;
        result.rows.push_back(
            {"x" + std::to_string(column + 1), formatNumber(detAi), formatNumber(xi)});
    }

    result.converged = true;
    result.message = size > 4
        ? "Solved with Cramer's Rule, but this method is impractical for larger systems."
        : "Solved with Cramer's Rule.";
    result.matrixSteps.push_back({"Original A matrix", config.matrixA});
    return result;
}

} // namespace numerical
