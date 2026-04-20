#include "SolverCommon.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "../parser/Evaluator.h"
#include "../parser/Tokenizer.h"
#include "../validation/Errors.h"
#include "../validation/Validator.h"

namespace numerical {

std::shared_ptr<AstNode> parseExpressionTree(const std::string &expression) {
    Validator::validateExpressionInput(expression);
    Tokenizer tokenizer;
    Parser parser(tokenizer.tokenize(expression));
    auto root = parser.parse();
    return std::shared_ptr<AstNode>(root.release());
}

std::function<double(double)> compileExpression(const std::string &expression) {
    return Evaluator::compile(parseExpressionTree(expression));
}

std::string formatNumber(double value, int precision) {
    if (!std::isfinite(value)) {
        return "—";
    }
    std::ostringstream stream;
    stream << std::setprecision(precision) << std::defaultfloat << value;
    return stream.str();
}

std::string formatVector(const std::vector<double> &values, int precision) {
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            stream << ", ";
        }
        stream << formatNumber(values[index], precision);
    }
    stream << ']';
    return stream.str();
}

double relativeApproxErrorPercent(double current, double previous) {
    const double denominator = std::max(std::abs(current), 1e-15);
    return std::abs((current - previous) / denominator) * 100.0;
}

std::vector<PlotPoint> sampleFunction(const std::function<double(double)> &function,
                                      double start,
                                      double end,
                                      int samples) {
    if (end < start) {
        std::swap(start, end);
    }
    if (std::abs(end - start) < 1e-12) {
        end = start + 1.0;
    }

    std::vector<PlotPoint> points;
    points.reserve(static_cast<std::size_t>(samples));
    const double step = (end - start) / static_cast<double>(samples - 1);
    for (int i = 0; i < samples; ++i) {
        const double x = start + (step * static_cast<double>(i));
        try {
            const double y = function(x);
            if (std::isfinite(y)) {
                points.push_back({x, y});
            }
        } catch (const std::exception &) {
        }
    }
    return points;
}

std::vector<std::vector<double>> makeAugmentedMatrix(
    const std::vector<std::vector<double>> &matrix,
    const std::vector<double> &rhs) {
    std::vector<std::vector<double>> augmented = matrix;
    for (std::size_t row = 0; row < augmented.size(); ++row) {
        augmented[row].push_back(rhs[row]);
    }
    return augmented;
}

double determinant(std::vector<std::vector<double>> matrix) {
    const int size = static_cast<int>(matrix.size());
    double det = 1.0;
    int sign = 1;

    for (int pivot = 0; pivot < size; ++pivot) {
        int maxRow = pivot;
        for (int row = pivot + 1; row < size; ++row) {
            if (std::abs(matrix[row][pivot]) > std::abs(matrix[maxRow][pivot])) {
                maxRow = row;
            }
        }

        if (std::abs(matrix[maxRow][pivot]) < 1e-12) {
            return 0.0;
        }

        if (maxRow != pivot) {
            std::swap(matrix[maxRow], matrix[pivot]);
            sign *= -1;
        }

        det *= matrix[pivot][pivot];
        for (int row = pivot + 1; row < size; ++row) {
            const double factor = matrix[row][pivot] / matrix[pivot][pivot];
            for (int column = pivot; column < size; ++column) {
                matrix[row][column] -= factor * matrix[pivot][column];
            }
        }
    }

    return det * static_cast<double>(sign);
}

std::vector<std::vector<double>> identityMatrix(int size) {
    std::vector<std::vector<double>> matrix(size, std::vector<double>(size, 0.0));
    for (int index = 0; index < size; ++index) {
        matrix[index][index] = 1.0;
    }
    return matrix;
}

std::vector<double> forwardSubstitute(const std::vector<std::vector<double>> &lower,
                                      const std::vector<double> &rhs) {
    const int size = static_cast<int>(lower.size());
    std::vector<double> solution(size, 0.0);
    for (int row = 0; row < size; ++row) {
        double sum = rhs[row];
        for (int column = 0; column < row; ++column) {
            sum -= lower[row][column] * solution[column];
        }
        if (std::abs(lower[row][row]) < 1e-12) {
            throw MathError("Lower triangular solve encountered a zero pivot.");
        }
        solution[row] = sum / lower[row][row];
    }
    return solution;
}

std::vector<double> backSubstitute(const std::vector<std::vector<double>> &upper,
                                   const std::vector<double> &rhs) {
    const int size = static_cast<int>(upper.size());
    std::vector<double> solution(size, 0.0);
    for (int row = size - 1; row >= 0; --row) {
        double sum = rhs[row];
        for (int column = row + 1; column < size; ++column) {
            sum -= upper[row][column] * solution[column];
        }
        if (std::abs(upper[row][row]) < 1e-12) {
            throw MathError("Upper triangular solve encountered a zero pivot.");
        }
        solution[row] = sum / upper[row][row];
    }
    return solution;
}

std::vector<double> multiply(const std::vector<std::vector<double>> &matrix,
                             const std::vector<double> &vector) {
    std::vector<double> result(matrix.size(), 0.0);
    for (std::size_t row = 0; row < matrix.size(); ++row) {
        for (std::size_t column = 0; column < vector.size(); ++column) {
            result[row] += matrix[row][column] * vector[column];
        }
    }
    return result;
}

std::vector<double> add(const std::vector<double> &lhs,
                        const std::vector<double> &rhs) {
    std::vector<double> result(lhs.size(), 0.0);
    for (std::size_t index = 0; index < lhs.size(); ++index) {
        result[index] = lhs[index] + rhs[index];
    }
    return result;
}

std::vector<double> subtract(const std::vector<double> &lhs,
                             const std::vector<double> &rhs) {
    std::vector<double> result(lhs.size(), 0.0);
    for (std::size_t index = 0; index < lhs.size(); ++index) {
        result[index] = lhs[index] - rhs[index];
    }
    return result;
}

std::vector<double> scale(const std::vector<double> &values, double factor) {
    std::vector<double> result(values.size(), 0.0);
    for (std::size_t index = 0; index < values.size(); ++index) {
        result[index] = values[index] * factor;
    }
    return result;
}

double dot(const std::vector<double> &lhs, const std::vector<double> &rhs) {
    return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), 0.0);
}

double norm(const std::vector<double> &values) {
    return std::sqrt(dot(values, values));
}

bool isSymmetric(const std::vector<std::vector<double>> &matrix,
                 double tolerance) {
    for (std::size_t row = 0; row < matrix.size(); ++row) {
        for (std::size_t column = row + 1; column < matrix[row].size(); ++column) {
            if (std::abs(matrix[row][column] - matrix[column][row]) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

std::vector<double> permuteVector(const std::vector<double> &values,
                                  const std::vector<std::vector<double>> &permutation) {
    return multiply(permutation, values);
}

} // namespace numerical
