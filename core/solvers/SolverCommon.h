#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../SolverResult.h"
#include "../parser/Parser.h"

namespace numerical {

std::shared_ptr<AstNode> parseExpressionTree(const std::string &expression);
std::function<double(double)> compileExpression(const std::string &expression);
std::string formatNumber(double value, int precision = 6);
std::string formatVector(const std::vector<double> &values, int precision = 6);
double relativeApproxErrorPercent(double current, double previous);
std::vector<PlotPoint> sampleFunction(const std::function<double(double)> &function,
                                      double start,
                                      double end,
                                      int samples = 200);
std::vector<std::vector<double>> makeAugmentedMatrix(
    const std::vector<std::vector<double>> &matrix,
    const std::vector<double> &rhs);
double determinant(std::vector<std::vector<double>> matrix);
std::vector<std::vector<double>> identityMatrix(int size);
std::vector<double> forwardSubstitute(const std::vector<std::vector<double>> &lower,
                                      const std::vector<double> &rhs);
std::vector<double> backSubstitute(const std::vector<std::vector<double>> &upper,
                                   const std::vector<double> &rhs);
std::vector<double> multiply(const std::vector<std::vector<double>> &matrix,
                             const std::vector<double> &vector);
std::vector<double> add(const std::vector<double> &lhs,
                        const std::vector<double> &rhs);
std::vector<double> subtract(const std::vector<double> &lhs,
                             const std::vector<double> &rhs);
std::vector<double> scale(const std::vector<double> &values, double factor);
double dot(const std::vector<double> &lhs, const std::vector<double> &rhs);
double norm(const std::vector<double> &values);
bool isSymmetric(const std::vector<std::vector<double>> &matrix,
                 double tolerance = 1e-9);
std::vector<double> permuteVector(const std::vector<double> &values,
                                  const std::vector<std::vector<double>> &permutation);

} // namespace numerical
