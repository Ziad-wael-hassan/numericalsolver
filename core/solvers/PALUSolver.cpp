#include "PALUSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult PALUSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);

    const int size = static_cast<int>(config.matrixA.size());
    auto upper = config.matrixA;
    auto lower = identityMatrix(size);
    auto permutation = identityMatrix(size);

    for (int pivot = 0; pivot < size; ++pivot) {
        int maxRow = pivot;
        for (int row = pivot + 1; row < size; ++row) {
            if (std::abs(upper[row][pivot]) > std::abs(upper[maxRow][pivot])) {
                maxRow = row;
            }
        }

        if (std::abs(upper[maxRow][pivot]) < 1e-12) {
            throw MathError("Matrix is singular (det = 0). The system has no unique solution.");
        }

        if (maxRow != pivot) {
            std::swap(upper[maxRow], upper[pivot]);
            std::swap(permutation[maxRow], permutation[pivot]);
            for (int column = 0; column < pivot; ++column) {
                std::swap(lower[maxRow][column], lower[pivot][column]);
            }
        }

        for (int row = pivot + 1; row < size; ++row) {
            const double factor = upper[row][pivot] / upper[pivot][pivot];
            lower[row][pivot] = factor;
            for (int column = pivot; column < size; ++column) {
                upper[row][column] -= factor * upper[pivot][column];
            }
        }
    }

    const auto permutedB = permuteVector(config.vectorB, permutation);
    const auto y = forwardSubstitute(lower, permutedB);
    const auto x = backSubstitute(upper, y);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::PALUFactorization);
    result.converged = true;
    result.vectorResult = x;
    result.headers = {"Component", "Values"};
    result.rows = {
        {"Pb", formatVector(permutedB)},
        {"y", formatVector(y)},
        {"x", formatVector(x)},
    };
    result.matrixSteps = {
        {"P matrix", permutation},
        {"L matrix", lower},
        {"U matrix", upper},
    };
    result.message = "PA = LU factorization with partial pivoting completed successfully.";
    return result;
}

} // namespace numerical
