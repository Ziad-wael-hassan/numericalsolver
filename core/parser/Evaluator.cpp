#include "Evaluator.h"

#include <cmath>
#include <utility>

#include "../validation/Errors.h"

namespace numerical {

namespace {

double applyFunction(const std::string &name, double value) {
    if (name == "sin") {
        return std::sin(value);
    }
    if (name == "cos") {
        return std::cos(value);
    }
    if (name == "tan") {
        return std::tan(value);
    }
    if (name == "exp") {
        return std::exp(value);
    }
    if (name == "log") {
        if (value <= 0.0) {
            throw MathError("Logarithm domain error.");
        }
        return std::log(value);
    }
    if (name == "log10") {
        if (value <= 0.0) {
            throw MathError("Base-10 logarithm domain error.");
        }
        return std::log10(value);
    }
    if (name == "sqrt") {
        if (value < 0.0) {
            throw MathError("Square root domain error.");
        }
        return std::sqrt(value);
    }
    if (name == "abs") {
        return std::abs(value);
    }

    throw ParseError("Unsupported function: " + name + "(x)");
}

} // namespace

double Evaluator::evaluate(const AstNode &node, double x) {
    switch (node.kind) {
    case AstNode::Kind::Number:
        return node.numberValue;
    case AstNode::Kind::Variable:
        return x;
    case AstNode::Kind::Unary: {
        const double value = evaluate(*node.left, x);
        return node.text == "-" ? -value : value;
    }
    case AstNode::Kind::Binary: {
        const double lhs = evaluate(*node.left, x);
        const double rhs = evaluate(*node.right, x);
        if (node.text == "+") {
            return lhs + rhs;
        }
        if (node.text == "-") {
            return lhs - rhs;
        }
        if (node.text == "*") {
            return lhs * rhs;
        }
        if (node.text == "/") {
            if (std::abs(rhs) < 1e-15) {
                throw MathError("Division by zero encountered during evaluation.");
            }
            return lhs / rhs;
        }
        if (node.text == "^") {
            return std::pow(lhs, rhs);
        }
        throw ParseError("Unsupported operator: " + node.text);
    }
    case AstNode::Kind::Function:
        return applyFunction(node.text, evaluate(*node.left, x));
    }

    throw ParseError("Unsupported AST node.");
}

std::function<double(double)> Evaluator::compile(std::shared_ptr<AstNode> root) {
    return [root = std::move(root)](double x) {
        return evaluate(*root, x);
    };
}

} // namespace numerical
