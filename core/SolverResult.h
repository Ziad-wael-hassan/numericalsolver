#pragma once

#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace numerical {

enum class MethodId {
    Bisection,
    FalsePosition,
    NewtonRaphson,
    FixedPoint,
    Secant,
    GaussianElimination,
    LUFactorization,
    PALUFactorization,
    GaussJordan,
    Cramer,
    GoldenSection,
    ConjugateGradient,
};

enum class MethodCategory {
    RootFinding,
    LinearSystem,
    Optimization,
};

inline MethodCategory categoryFor(MethodId methodId) {
    switch (methodId) {
    case MethodId::Bisection:
    case MethodId::FalsePosition:
    case MethodId::NewtonRaphson:
    case MethodId::FixedPoint:
    case MethodId::Secant:
        return MethodCategory::RootFinding;
    case MethodId::GaussianElimination:
    case MethodId::LUFactorization:
    case MethodId::PALUFactorization:
    case MethodId::GaussJordan:
    case MethodId::Cramer:
        return MethodCategory::LinearSystem;
    case MethodId::GoldenSection:
    case MethodId::ConjugateGradient:
        return MethodCategory::Optimization;
    }
    return MethodCategory::RootFinding;
}

inline std::string toDisplayName(MethodId methodId) {
    switch (methodId) {
    case MethodId::Bisection:
        return "Bisection Method";
    case MethodId::FalsePosition:
        return "False Position Method";
    case MethodId::NewtonRaphson:
        return "Newton-Raphson Method";
    case MethodId::FixedPoint:
        return "Fixed-Point Iteration";
    case MethodId::Secant:
        return "Secant Method";
    case MethodId::GaussianElimination:
        return "Gaussian Elimination";
    case MethodId::LUFactorization:
        return "LU Factorization";
    case MethodId::PALUFactorization:
        return "PA = LU Factorization";
    case MethodId::GaussJordan:
        return "Gauss-Jordan Elimination";
    case MethodId::Cramer:
        return "Cramer's Rule";
    case MethodId::GoldenSection:
        return "Golden-Section Search";
    case MethodId::ConjugateGradient:
        return "Conjugate Gradient Method";
    }
    return "Unknown Method";
}

struct PlotPoint {
    double x {};
    double y {};
};

struct MatrixStep {
    std::string label;
    std::vector<std::vector<double>> matrix;
};

struct SolverConfig {
    MethodId methodId {MethodId::Bisection};
    std::string functionExpression;
    std::string derivativeExpression;
    std::string fixedPointExpression;
    double intervalStart {0.0};
    double intervalEnd {0.0};
    double initialGuess {0.0};
    double secondInitialGuess {0.0};
    double tolerance {1e-4};
    int maxIterations {100};
    bool showAllIterations {true};
    bool maximize {false};
    std::vector<std::vector<double>> matrixA;
    std::vector<double> vectorB;
    std::vector<double> initialVector;
};

struct SolverResult {
    std::string methodName;
    bool converged {false};
    int iterations {0};
    double scalarResult {std::numeric_limits<double>::quiet_NaN()};
    double optimumValue {std::numeric_limits<double>::quiet_NaN()};
    std::vector<double> vectorResult;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    std::vector<MatrixStep> matrixSteps;
    std::vector<PlotPoint> functionSeries;
    std::vector<PlotPoint> convergenceSeries;
    std::vector<PlotPoint> markers;
    double achievedTolerance {std::numeric_limits<double>::quiet_NaN()};
    std::string message;
};

} // namespace numerical
