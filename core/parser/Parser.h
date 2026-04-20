#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Tokenizer.h"

namespace numerical {

struct AstNode {
    enum class Kind {
        Number,
        Variable,
        Unary,
        Binary,
        Function,
    };

    Kind kind {Kind::Number};
    double numberValue {0.0};
    std::string text;
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<AstNode> parse();

private:
    const Token &current() const;
    bool match(TokenType type, const std::string &text = "");
    const Token &consume(TokenType type, const std::string &message);
    std::unique_ptr<AstNode> parseExpression();
    std::unique_ptr<AstNode> parseTerm();
    std::unique_ptr<AstNode> parseUnary();
    std::unique_ptr<AstNode> parseFactor();
    std::unique_ptr<AstNode> parsePrimary();
    [[noreturn]] void throwUnexpected(const std::string &message) const;

    std::vector<Token> tokens_;
    std::size_t index_ {0};
};

} // namespace numerical
