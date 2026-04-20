#include "Parser.h"

#include <utility>

#include "../validation/Errors.h"

namespace numerical {

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {}

std::unique_ptr<AstNode> Parser::parse() {
    auto root = parseExpression();
    if (current().type != TokenType::End) {
        throwUnexpected("unexpected token");
    }
    return root;
}

const Token &Parser::current() const {
    return tokens_.at(index_);
}

bool Parser::match(TokenType type, const std::string &text) {
    if (current().type != type) {
        return false;
    }
    if (!text.empty() && current().text != text) {
        return false;
    }
    ++index_;
    return true;
}

const Token &Parser::consume(TokenType type, const std::string &message) {
    if (current().type != type) {
        throwUnexpected(message);
    }
    return tokens_.at(index_++);
}

std::unique_ptr<AstNode> Parser::parseExpression() {
    auto node = parseTerm();

    while (current().type == TokenType::Operator &&
           (current().text == "+" || current().text == "-")) {
        auto op = current().text;
        ++index_;

        auto next = std::make_unique<AstNode>();
        next->kind = AstNode::Kind::Binary;
        next->text = op;
        next->left = std::move(node);
        next->right = parseTerm();
        node = std::move(next);
    }

    return node;
}

std::unique_ptr<AstNode> Parser::parseTerm() {
    auto node = parseUnary();

    while (current().type == TokenType::Operator &&
           (current().text == "*" || current().text == "/")) {
        auto op = current().text;
        ++index_;

        auto next = std::make_unique<AstNode>();
        next->kind = AstNode::Kind::Binary;
        next->text = op;
        next->left = std::move(node);
        next->right = parseUnary();
        node = std::move(next);
    }

    return node;
}

std::unique_ptr<AstNode> Parser::parseUnary() {
    if (current().type == TokenType::Operator &&
        (current().text == "+" || current().text == "-")) {
        auto node = std::make_unique<AstNode>();
        node->kind = AstNode::Kind::Unary;
        node->text = current().text;
        ++index_;
        node->left = parseUnary();
        return node;
    }

    return parseFactor();
}

std::unique_ptr<AstNode> Parser::parseFactor() {
    auto node = parsePrimary();

    if (current().type == TokenType::Operator && current().text == "^") {
        ++index_;
        auto next = std::make_unique<AstNode>();
        next->kind = AstNode::Kind::Binary;
        next->text = "^";
        next->left = std::move(node);
        next->right = parseFactor();
        return next;
    }

    return node;
}

std::unique_ptr<AstNode> Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        auto node = std::make_unique<AstNode>();
        node->kind = AstNode::Kind::Number;
        node->numberValue = tokens_.at(index_ - 1).value;
        node->text = tokens_.at(index_ - 1).text;
        return node;
    }

    if (match(TokenType::Variable)) {
        auto node = std::make_unique<AstNode>();
        node->kind = AstNode::Kind::Variable;
        node->text = "x";
        return node;
    }

    if (match(TokenType::Function)) {
        const auto functionName = tokens_.at(index_ - 1).text;
        consume(TokenType::LeftParen, "expected '(' after function name");
        auto argument = parseExpression();
        consume(TokenType::RightParen, "missing closing parenthesis");

        auto node = std::make_unique<AstNode>();
        node->kind = AstNode::Kind::Function;
        node->text = functionName;
        node->left = std::move(argument);
        return node;
    }

    if (match(TokenType::LeftParen)) {
        auto node = parseExpression();
        consume(TokenType::RightParen, "missing closing parenthesis");
        return node;
    }

    throwUnexpected("expected a number, variable, or function");
}

void Parser::throwUnexpected(const std::string &message) const {
    throw ParseError("Parse error: " + message + " at position " +
                     std::to_string(current().position));
}

} // namespace numerical
