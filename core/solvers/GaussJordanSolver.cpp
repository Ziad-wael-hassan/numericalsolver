#include "GaussJordanSolver.h"

#include <algorithm>
#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult GaussJordanSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);

    const int size = static_cast<int>(config.matrixA.size());
    auto augmented = makeAugmentedMatrix(config.matrixA, config.vectorB);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::GaussJordan);
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
            result.matrixSteps.push_back({"After row swap", augmented});
        }

        const double pivotValue = augmented[pivot][pivot];
        for (int column = 0; column <= size; ++column) {
            augmented[pivot][column] /= pivotValue;
        }
        result.rows.push_back(
            {std::to_string(step++),
             "Normalize R" + std::to_string(pivot + 1)});
        result.matrixSteps.push_back({"After normalization", augmented});

        for (int row = 0; row < size; ++row) {
            if (row == pivot) {
                continue;
            }
            const double factor = augmented[row][pivot];
            for (int column = 0; column <= size; ++column) {
                augmented[row][column] -= factor * augmented[pivot][column];
            }
            result.rows.push_back(
                {std::to_string(step++),
                 "R" + std::to_string(row + 1) + " = R" + std::to_string(row + 1) +
                     " - (" + formatNumber(factor) + ")R" + std::to_string(pivot + 1)});
            result.matrixSteps.push_back({"After elimination", augmented});
        }
    }

    result.vectorResult.resize(static_cast<std::size_t>(size));
    for (int row = 0; row < size; ++row) {
        result.vectorResult[row] = augmented[row][size];
    }
    result.converged = true;
    result.message = "Reduced row echelon form reached successfully.";
    return result;
}

} // namespace numerical
