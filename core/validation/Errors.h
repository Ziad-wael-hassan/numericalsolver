#pragma once

#include <stdexcept>
#include <string>

namespace numerical {

struct SolverError : std::runtime_error {
    explicit SolverError(const std::string &message)
        : std::runtime_error(message) {}
};

struct ValidationError : SolverError {
    explicit ValidationError(const std::string &message)
        : SolverError(message) {}
};

struct ParseError : SolverError {
    explicit ParseError(const std::string &message)
        : SolverError(message) {}
};

struct MathError : SolverError {
    explicit MathError(const std::string &message)
        : SolverError(message) {}
};

struct ConvergenceError : SolverError {
    explicit ConvergenceError(const std::string &message)
        : SolverError(message) {}
};

struct PlotError : SolverError {
    explicit PlotError(const std::string &message)
        : SolverError(message) {}
};

} // namespace numerical
