#include "Tokenizer.h"

#include <cctype>
#include <string_view>

#include "../validation/Errors.h"

namespace numerical {

namespace {

bool isIdentifierStart(char ch) {
    return static_cast<bool>(std::isalpha(static_cast<unsigned char>(ch)));
}

bool isIdentifierChar(char ch) {
    return static_cast<bool>(std::isalnum(static_cast<unsigned char>(ch)) ||
                             ch == '_');
}

} // namespace

std::vector<Token> Tokenizer::tokenize(const std::string &expression) const {
    std::vector<Token> tokens;
    std::size_t index = 0;

    while (index < expression.size()) {
        const char current = expression[index];
        if (std::isspace(static_cast<unsigned char>(current)) != 0) {
            ++index;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(current)) != 0 ||
            current == '.') {
            const std::size_t start = index;
            bool sawExponent = false;
            bool sawDot = (current == '.');
            ++index;

            while (index < expression.size()) {
                const char ch = expression[index];
                if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
                    ++index;
                    continue;
                }
                if (ch == '.' && !sawDot && !sawExponent) {
                    sawDot = true;
                    ++index;
                    continue;
                }
                if ((ch == 'e' || ch == 'E') && !sawExponent) {
                    sawExponent = true;
                    ++index;
                    if (index < expression.size() &&
                        (expression[index] == '+' || expression[index] == '-')) {
                        ++index;
                    }
                    continue;
                }
                break;
            }

            const auto text = expression.substr(start, index - start);
            try {
                tokens.push_back(Token {
                    TokenType::Number,
                    text,
                    std::stod(text),
                    start,
                });
            } catch (const std::exception &) {
                throw ParseError("Parse error: invalid number at position " +
                                 std::to_string(start));
            }
            continue;
        }

        if (isIdentifierStart(current)) {
            const std::size_t start = index;
            ++index;
            while (index < expression.size() &&
                   isIdentifierChar(expression[index])) {
                ++index;
            }

            const auto identifier = expression.substr(start, index - start);
            if (identifier == "x") {
                tokens.push_back(Token {TokenType::Variable, identifier, 0.0, start});
            } else if (identifier == "pi") {
                tokens.push_back(Token {TokenType::Number, identifier, 3.14159265358979323846, start});
            } else if (identifier == "e") {
                tokens.push_back(Token {TokenType::Number, identifier, 2.71828182845904523536, start});
            } else {
                tokens.push_back(Token {TokenType::Function, identifier, 0.0, start});
            }
            continue;
        }

        switch (current) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
            tokens.push_back(Token {TokenType::Operator, std::string(1, current), 0.0, index});
            ++index;
            break;
        case '(':
            tokens.push_back(Token {TokenType::LeftParen, "(", 0.0, index});
            ++index;
            break;
        case ')':
            tokens.push_back(Token {TokenType::RightParen, ")", 0.0, index});
            ++index;
            break;
        case ',':
            tokens.push_back(Token {TokenType::Comma, ",", 0.0, index});
            ++index;
            break;
        default:
            throw ParseError("Parse error: unsupported character '" +
                             std::string(1, current) + "' at position " +
                             std::to_string(index));
        }
    }

    tokens.push_back(Token {TokenType::End, "", 0.0, expression.size()});
    return tokens;
}

} // namespace numerical
