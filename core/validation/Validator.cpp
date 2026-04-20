#include "Validator.h"

#include <cmath>
#include <regex>
#include <string_view>
#include <unordered_set>

#include "Errors.h"

namespace numerical {

namespace {

bool containsBannedPattern(std::string_view expression) {
    static const std::unordered_set<std::string> bannedPatterns {
        ";",
        "{",
        "}",
        "#include",
        "system",
        "exec",
        "__",
    };

    for (const auto &pattern : bannedPatterns) {
        if (expression.find(pattern) != std::string_view::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

void Validator::validateExpressionInput(const std::string &expression,
                                        bool allowEmpty) {
    if (expression.empty()) {
        if (allowEmpty) {
            return;
        }
        throw ValidationError("Expression is required.");
    }

    if (expression.size() > 500U) {
        throw ValidationError("Expression too long.");
    }

    if (containsBannedPattern(expression)) {
        throw ValidationError("Invalid character sequence in expression.");
    }

    static const std::regex allowedPattern {
        R"(^[0-9a-zA-Z+\-*/^()._, ]+$)"
    };

    if (!std::regex_match(expression, allowedPattern)) {
        throw ValidationError(
            "Unsupported characters. Allowed: letters, digits, + - * / ^ ( ) . _ , and spaces.");
    }
}

void Validator::validateInterval(double a, double b) {
    validateFinite(a, "Interval start");
    validateFinite(b, "Interval end");
    if (a >= b) {
        throw ValidationError("Interval must satisfy a < b.");
    }
}

void Validator::validateTolerance(double tolerance) {
    validateFinite(tolerance, "Tolerance");
    if (tolerance <= 0.0 || tolerance >= 1.0) {
        throw ValidationError("Tolerance must be greater than 0 and less than 1.");
    }
}

void Validator::validateMaxIterations(int maxIterations) {
    if (maxIterations <= 0) {
        throw ValidationError("Maximum iterations must be positive.");
    }
}

void Validator::validateFinite(double value, const std::string &fieldName) {
    if (!std::isfinite(value)) {
        throw ValidationError(fieldName + " must be a finite number.");
    }
}

void Validator::validateMatrixSystem(
    const std::vector<std::vector<double>> &matrix,
    const std::vector<double> &rhs,
    int minSize,
    int maxSize) {
    const auto size = static_cast<int>(matrix.size());
    if (size < minSize || size > maxSize) {
        throw ValidationError("Matrix size must be between 2 and 10.");
    }

    if (rhs.size() != matrix.size()) {
        throw ValidationError("Right-hand side vector size must match matrix size.");
    }

    for (int row = 0; row < size; ++row) {
        if (static_cast<int>(matrix[row].size()) != size) {
            throw ValidationError("Matrix must be square.");
        }
        for (int column = 0; column < size; ++column) {
            validateFinite(matrix[row][column], "Matrix entry");
        }
        validateFinite(rhs[row], "Right-hand side value");
    }
}

} // namespace numerical
