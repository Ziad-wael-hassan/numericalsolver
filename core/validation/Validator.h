#pragma once

#include <string>
#include <vector>

namespace numerical {

class Validator {
public:
    static void validateExpressionInput(const std::string &expression,
                                        bool allowEmpty = false);
    static void validateInterval(double a, double b);
    static void validateTolerance(double tolerance);
    static void validateMaxIterations(int maxIterations);
    static void validateFinite(double value, const std::string &fieldName);
    static void validateMatrixSystem(
        const std::vector<std::vector<double>> &matrix,
        const std::vector<double> &rhs,
        int minSize = 2,
        int maxSize = 10);
};

} // namespace numerical
