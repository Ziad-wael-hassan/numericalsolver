#pragma once

#include <string>
#include <vector>

namespace numerical {

enum class TokenType {
    Number,
    Variable,
    Function,
    Operator,
    LeftParen,
    RightParen,
    Comma,
    End,
};

struct Token {
    TokenType type {TokenType::End};
    std::string text;
    double value {0.0};
    std::size_t position {0};
};

class Tokenizer {
public:
    std::vector<Token> tokenize(const std::string &expression) const;
};

} // namespace numerical
