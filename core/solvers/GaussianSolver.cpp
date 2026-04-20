#include "GaussianSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult GaussianSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);

    auto augmented = makeAugmentedMatrix(config.matrixA, config.vectorB);
    const int size = static_cast<int>(config.matrixA.size());

    SolverResult result;
    result.methodName = toDisplayName(MethodId::GaussianElimination);
    result.headers = {"Step", "Description"};
    result.matrixSteps.push_back({"Initial augmented matrix", augmented});

    int step = 1;
    for (int pivot = 0; pivot < size; ++pivot) {
        int maxRow = pivot;
        for (int row = pivot + 1; row < size; ++row) {
            if (std::abs(augmented[row][pivot]) > std::abs(augmented[maxRow][pivot])) {
                maxRow = row;
            }
        }

        if (std::abs(augmented[maxRow][pivot]) < 1e-12) {
            throw MathError("Matrix is singular (det = 0). The system has no unique solution.");
        }

        if (maxRow != pivot) {
            std::swap(augmented[maxRow], augmented[pivot]);
            result.rows.push_back(
                {std::to_string(step++),
                 "Swap R" + std::to_string(pivot + 1) + " with R" + std::to_string(maxRow + 1)});
            result.matrixSteps.push_back(
                {"After row swap", augmented});
        }

        for (int row = pivot + 1; row < size; ++row) {
            const double factor = augmented[row][pivot] / augmented[pivot][pivot];
            for (int column = pivot; column <= size; ++column) {
                augmented[row][column] -= factor * augmented[pivot][column];
            }
            result.rows.push_back(
                {std::to_string(step++),
                 "R" + std::to_string(row + 1) + " = R" + std::to_string(row + 1) +
                     " - (" + formatNumber(factor) + ")R" + std::to_string(pivot + 1)});
            result.matrixSteps.push_back(
                {"After elimination step", augmented});
        }
    }

    std::vector<std::vector<double>> upper(size, std::vector<double>(size, 0.0));
    std::vector<double> rhs(size, 0.0);
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            upper[row][column] = augmented[row][column];
        }
        rhs[row] = augmented[row][size];
    }

    result.vectorResult = backSubstitute(upper, rhs);
    result.converged = true;
    result.message = "Solved with Gaussian elimination and partial pivoting.";
    result.rows.push_back({std::to_string(step), "Back substitution gives x = " + formatVector(result.vectorResult)});
    return result;
}

} // namespace numerical
