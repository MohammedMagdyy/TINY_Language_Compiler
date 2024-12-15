#include "Parser.h"
#include <stdexcept>

using namespace std;

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

Token Parser::currentToken() {
    return tokens[currentTokenIndex];
}

void Parser::advance() {
    if (currentTokenIndex < tokens.size() - 1) {
        currentTokenIndex++;
    }
}

void Parser::match(const std::string& expectedType) {
    if (currentToken().type == expectedType) {
        advance();
    } else {
        throw runtime_error(
            "Error at line " + to_string(currentToken().line) + " : Unexpected token \"" +
            currentToken().type + "\", expected \"" + expectedType + "\"");
    }
}

shared_ptr<TreeNode> Parser::program() {
    return stmt_sequence();
}

shared_ptr<TreeNode> Parser::stmt_sequence() {
    auto node = statement();

    while (currentToken().type == "SEMICOLON") {
        match("SEMICOLON");
        node->siblings.push_back(statement());
    }
    return node;
}

shared_ptr<TreeNode> Parser::statement() {
    if (currentToken().type == "IF") return if_stmt();
    if (currentToken().type == "REPEAT") return repeat_stmt();
    if (currentToken().type == "IDENTIFIER") return assign_stmt();
    if (currentToken().type == "READ") return read_stmt();
    if (currentToken().type == "WRITE") return write_stmt();

    throw runtime_error(
        "Error at line " + to_string(currentToken().line) + " : Invalid statement \"" + currentToken().value + "\"");
}

shared_ptr<TreeNode> Parser::if_stmt() {
    auto node = std::make_shared<TreeNode>("if");
    match("IF");
    node->children.push_back(exp());
    match("THEN");
    node->children.push_back(stmt_sequence());

    if (currentToken().type == "ELSE") {
        match("ELSE");
        node->children.push_back(stmt_sequence());
    }

    match("END");
    return node;
}

shared_ptr<TreeNode> Parser::repeat_stmt() {
    auto node = std::make_shared<TreeNode>("repeat");
    match("REPEAT");
    node->children.push_back(stmt_sequence());
    match("UNTIL");
    node->children.push_back(exp());
    return node;
}

shared_ptr<TreeNode> Parser::assign_stmt() {
    auto node = std::make_shared<TreeNode>("assign", currentToken().value);
    match("IDENTIFIER");
    match("ASSIGN");
    node->children.push_back(exp());
    return node;
}

shared_ptr<TreeNode> Parser::read_stmt() {
    match("READ");
    auto node = std::make_shared<TreeNode>("read", currentToken().value);
    match("IDENTIFIER");
    return node;
}

shared_ptr<TreeNode> Parser::write_stmt() {
    auto node = std::make_shared<TreeNode>("write");
    match("WRITE");
    node->children.push_back(exp());
    return node;
}

shared_ptr<TreeNode> Parser::exp() {
    auto node = simple_exp();

    if (currentToken().type == "LESSTHAN" || currentToken().type == "EQUAL") {
        auto opNode = std::make_shared<TreeNode>("op", currentToken().value);
        match(currentToken().type);
        opNode->children.push_back(node);
        opNode->children.push_back(simple_exp());
        node = opNode;
    }
    return node;
}

shared_ptr<TreeNode> Parser::simple_exp() {
    auto node = term();

    while (currentToken().type == "PLUS" || currentToken().type == "MINUS") {
        auto opNode = std::make_shared<TreeNode>("op", currentToken().value);
        match(currentToken().type);
        opNode->children.push_back(node);
        opNode->children.push_back(term());
        node = opNode;
    }
    return node;
}

shared_ptr<TreeNode> Parser::term() {
    auto node = factor();

    while (currentToken().type == "MULT" || currentToken().type == "DIV") {
        auto opNode = std::make_shared<TreeNode>("op", currentToken().value);
        match(currentToken().type);
        opNode->children.push_back(node);
        opNode->children.push_back(factor());
        node = opNode;
    }
    return node;
}

shared_ptr<TreeNode> Parser::factor() {
    if (currentToken().type == "NUMBER") {
        auto node = std::make_shared<TreeNode>("Const", currentToken().value);
        match("NUMBER");
        return node;
    }
    if (currentToken().type == "IDENTIFIER") {
        auto node = std::make_shared<TreeNode>("id", currentToken().value);
        match("IDENTIFIER");
        return node;
    }
    if (currentToken().type == "OPENBRACKET") {
        match("OPENBRACKET");
        auto node = exp();
        match("CLOSEDBRACKET");
        return node;
    }

    throw runtime_error(
        "Error at line " + to_string(currentToken().line) + " : Invalid factor \"" + currentToken().value + "\"");
}


shared_ptr<TreeNode> Parser :: parse() {
    return program();
}