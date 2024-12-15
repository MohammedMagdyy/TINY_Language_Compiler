#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include "Token.h"
#include "TreeNode.h"

using namespace std;

class Parser {
private:
    std::vector<Token> tokens;
    size_t currentTokenIndex;

    shared_ptr<TreeNode> program();
    shared_ptr<TreeNode> stmt_sequence();
    shared_ptr<TreeNode> statement();
    shared_ptr<TreeNode> if_stmt();
    shared_ptr<TreeNode> repeat_stmt();
    shared_ptr<TreeNode> assign_stmt();
    shared_ptr<TreeNode> read_stmt();
    shared_ptr<TreeNode> write_stmt();
    shared_ptr<TreeNode> exp();
    shared_ptr<TreeNode> simple_exp();
    shared_ptr<TreeNode> term();
    shared_ptr<TreeNode> factor();

    void match(const std::string& expectedType);
    Token currentToken();
    void advance();

public:
    explicit Parser(const std::vector<Token>& tokens);
    shared_ptr<TreeNode> parse();
};

#endif // PARSER_H
