#pragma once

#include <functional>
#include <memory>

#include "Parser.h"

namespace numerical {

class Evaluator {
public:
    static double evaluate(const AstNode &node, double x);
    static std::function<double(double)> compile(std::shared_ptr<AstNode> root);
};

} // namespace numerical
