#include "LUSolver.h"

#include <cmath>

#include "SolverCommon.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

SolverResult LUSolver::solve(const SolverConfig &config) const {
    Validator::validateMatrixSystem(config.matrixA, config.vectorB);

    const int size = static_cast<int>(config.matrixA.size());
    std::vector<std::vector<double>> lower = identityMatrix(size);
    std::vector<std::vector<double>> upper(size, std::vector<double>(size, 0.0));

    for (int row = 0; row < size; ++row) {
        for (int column = row; column < size; ++column) {
            double sum = 0.0;
            for (int k = 0; k < row; ++k) {
                sum += lower[row][k] * upper[k][column];
            }
            upper[row][column] = config.matrixA[row][column] - sum;
        }

        if (std::abs(upper[row][row]) < 1e-12) {
            throw MathError("LU factorization encountered a zero pivot. Try PA = LU instead.");
        }

        for (int column = row + 1; column < size; ++column) {
            double sum = 0.0;
            for (int k = 0; k < row; ++k) {
                sum += lower[column][k] * upper[k][row];
            }
            lower[column][row] = (config.matrixA[column][row] - sum) / upper[row][row];
        }
    }

    const auto y = forwardSubstitute(lower, config.vectorB);
    const auto x = backSubstitute(upper, y);

    SolverResult result;
    result.methodName = toDisplayName(MethodId::LUFactorization);
    result.converged = true;
    result.vectorResult = x;
    result.headers = {"Component", "Values"};
    result.rows = {
        {"y", formatVector(y)},
        {"x", formatVector(x)},
    };
    result.matrixSteps = {
        {"L matrix", lower},
        {"U matrix", upper},
    };
    result.message = "Doolittle LU factorization completed successfully.";
    return result;
}

} // namespace numerical
