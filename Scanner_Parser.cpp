#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <memory>

using namespace std;

// Token struct to store token value and type
struct Token {
    string value;
    string type;
    int line;
};

/**
 * The Scanner part:
 *1) Scan code throw input file of code
 *2) output file have all tokens value and type
 *3) Throwing error with number line if catch any unknown word
 */

bool inComment = false; // Track if we are inside a comment

// List of keywords and symbols in the TINY language
unordered_map<string, string> tokenMap = {
    {";", "SEMICOLON"}, {"if", "IF"}, {"then", "THEN"}, {"end", "END"}, {"repeat", "REPEAT"},
    {"until", "UNTIL"}, {":=", "ASSIGN"}, {"read", "READ"}, {"write", "WRITE"},
    {"<", "LESSTHAN"}, {"=", "EQUAL"}, {"+", "PLUS"}, {"-", "MINUS"},
    {"*", "MULT"}, {"/", "DIV"}, {"(", "OPENBRACKET"}, {")", "CLOSEDBRACKET"}
};

// Check if a string is a number
bool isNumber(const string &str) {
    for (char c: str) {
        if (c < '0' || c > '9') return false; // Check if not a digit
    }
    return !str.empty();
}

// Check if a string is an identifier (letters only)
bool isIdentifier(const string &str) {
    for (char c: str) {
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')) return false; // Check if not a letter
    }
    return !str.empty();
}


vector<Token> tokenize(const string &line, int lineCount) {
    vector<Token> tokens;
    string currentWord;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        // Handle comment start
        if (c == '{') {
            inComment = true; // Enter comment mode
            continue;
        }

        // Handle comment end
        if (c == '}') {
            inComment = false; // Exit comment mode
            continue;
        }

        // Skip characters inside comments
        if (inComment) {
            continue;
        }

        // Check for whitespace to finalize a token
        if (c == ' ' || c == '\t' || c == '\n') {
            if (!currentWord.empty()) {
                // Add the word to tokens
                if (tokenMap.find(currentWord) != tokenMap.end()) {
                    tokens.push_back({currentWord, tokenMap[currentWord],lineCount});
                } else if (isNumber(currentWord)) {
                    tokens.push_back({currentWord, "NUMBER",lineCount});
                } else if (isIdentifier(currentWord)) {
                    tokens.push_back({currentWord, "IDENTIFIER",lineCount});
                } else {
                    throw runtime_error("Error at line " + to_string(lineCount) + "  :  Unknown token -> " + currentWord);
                }
                currentWord.clear();
            }
            continue;
        }

        // Handle symbols and operators
        if (ispunct(c)) {
            if (!currentWord.empty()) {
                // Process the current word before handling the symbol
                if (tokenMap.find(currentWord) != tokenMap.end()) {
                    tokens.push_back({currentWord, tokenMap[currentWord],lineCount});
                } else if (isNumber(currentWord)) {
                    tokens.push_back({currentWord, "NUMBER",lineCount});
                } else if (isIdentifier(currentWord)) {
                    tokens.push_back({currentWord, "IDENTIFIER",lineCount});
                } else {
                    throw runtime_error("Error at line " + to_string(lineCount) + "  :  Unknown token -> " + currentWord);
                }
                currentWord.clear();
            }

            // Handle compound symbols (e.g., ":=")
            if ((c == ':' || c == '<' || c == '>') && i + 1 < line.size() && line[i + 1] == '=') {
                tokens.push_back({string(1, c) + line[i + 1], tokenMap[string(1, c) + line[i + 1]],lineCount});
                ++i; // Skip the next character
            } else {
                // Handle single-character symbols
                string symbol(1, c);
                if (tokenMap.find(symbol) != tokenMap.end()) {
                    tokens.push_back({symbol, tokenMap[symbol],lineCount});
                } else {
                    throw runtime_error("Error at line " + to_string(lineCount) + "  :  Unknown token -> " + currentWord);
                }
            }
            continue;
        }

        // If part of a word, add to currentWord
        currentWord += c;
    }

    // Process the last word if there is one
    if (!currentWord.empty()) {
        if (tokenMap.find(currentWord) != tokenMap.end()) {
            tokens.push_back({currentWord, tokenMap[currentWord]});
        } else if (isNumber(currentWord)) {
            tokens.push_back({currentWord, "NUMBER",lineCount});
        } else if (isIdentifier(currentWord)) {
            tokens.push_back({currentWord, "IDENTIFIER",lineCount});
        } else {
            throw runtime_error("Error at line " + to_string(lineCount) + "  :  Unknown token -> " + currentWord);
        }
    }

    return tokens;
}

/**
 * The parse part:
 *1) Check code cy Grammar rules
 *2) Have all Implementation of Grammar rules of Tiny language
 *3) Throwing error if catch any error
 */

// Node structure for the syntax tree
struct TreeNode {
    string name; // Grammar rule or token
    string value; // Optional value for leaf nodes (e.g., constants, identifiers, operations)
    vector<shared_ptr<TreeNode> > children; // Child nodes

    // Constructor with only name
    explicit TreeNode(string name) : name(move(name)) {
    }

    // Constructor with name and value
    TreeNode(string name, string value) : name(move(name)), value(move(value)) {
    }
};


// Parser class
class Parser {
private:
    vector<Token> tokens; // List of tokens
    size_t currentTokenIndex;

    Token currentToken() {
        return tokens[currentTokenIndex];
    }

    void advance() {
        if (currentTokenIndex < tokens.size() - 1) {
            currentTokenIndex++;
        }
    }

    // Modified match function to only check and consume tokens
    void match(const string &expectedType) {
        if (currentToken().type == expectedType) {
            advance(); // Move to the next token
        } else {
            Token previousToken = tokens[currentTokenIndex - 1];
            throw runtime_error("Error at line " + to_string(previousToken.line) + " : Unexpected token: " + currentToken().type + ", expected: " + expectedType);
        }
    }

    // Grammar rule functions
    shared_ptr<TreeNode> program() {
        return stmt_sequence();
    }

    shared_ptr<TreeNode> stmt_sequence() {
        // Create the first statement
        auto node = make_shared<TreeNode>("Stmt-Sequence");
        node->children.push_back(statement());

        // Process additional statements, if any, separated by semicolons
        while (currentToken().type == "SEMICOLON") {
            match("SEMICOLON"); // Consume the semicolon
            node->children.push_back(statement());
        }
        return node;
    }

    shared_ptr<TreeNode> statement() {
        if (currentToken().type == "IF") return if_stmt();
        if (currentToken().type == "REPEAT") return repeat_stmt();
        if (currentToken().type == "IDENTIFIER") return assign_stmt();
        if (currentToken().type == "READ") return read_stmt();
        if (currentToken().type == "WRITE") return write_stmt();

        throw runtime_error("Error at line " + to_string(currentToken().line) + " : Invalid statement: " + currentToken().value);
    }

    shared_ptr<TreeNode> if_stmt() {
        auto node = make_shared<TreeNode>("if");
        match("IF");            // Match 'IF' keyword
        node->children.push_back(exp());    // Parse the conditional expression
        match("THEN");          // Match 'THEN' keyword
        node->children.push_back(stmt_sequence());        // Parse the statements in the 'THEN' block

        // Check for an optional 'ELSE' block
        if (currentToken().type == "ELSE") {
            match("ELSE");
            node->children.push_back(stmt_sequence());
        }
        match("END");         // Match 'END' keyword
        return node;
    }

    shared_ptr<TreeNode> repeat_stmt() {
        auto node = make_shared<TreeNode>("repeat");
        match("REPEAT");
        node->children.push_back(stmt_sequence());
        match("UNTIL");
        node->children.push_back(exp());
        return node;
    }

    shared_ptr<TreeNode> assign_stmt() {
        auto node = make_shared<TreeNode>("assign", currentToken().value);
        match("IDENTIFIER");
        match("ASSIGN");
        node->children.push_back(exp());
        return node;
    }

    shared_ptr<TreeNode> read_stmt() {
        auto node = make_shared<TreeNode>("read");
        match("READ");
        match("IDENTIFIER");
        return node;
    }

    shared_ptr<TreeNode> write_stmt() {
        auto node = make_shared<TreeNode>("write");
        match("WRITE");
        node->children.push_back(exp());
        return node;
    }

    shared_ptr<TreeNode> exp() {
        // Parse the left-hand side of the expression
        auto node = simple_exp();

        // Check for a comparison operator
        if (currentToken().type == "LESSTHAN" || currentToken().type == "EQUAL") {
            auto opNode = make_shared<TreeNode>("op", currentToken().value); // Create node for the operator
            match(currentToken().type); // Consume the comparison operator
            opNode->children.push_back(node); // Add the operator to the syntax tree
            opNode->children.push_back(simple_exp()); // Parse the right-hand side of the expression
            node = opNode;
        }
        return node;
    }

    shared_ptr<TreeNode> simple_exp() {
        auto node = term();

        // Handle optional addition/subtraction operations
        while (currentToken().type == "PLUS" || currentToken().type == "MINUS") {
            auto opNode = make_shared<TreeNode>("op", currentToken().value);
            match(currentToken().type); // Consume '+' or '-'
            opNode->children.push_back(node);
            opNode->children.push_back(term());
            node = opNode; // Update the node with the operation subtree
        }
        return node;
    }

    shared_ptr<TreeNode> term() {
        auto node = factor();

        // Handle optional addition/subtraction operations
        while (currentToken().type == "MULT" || currentToken().type == "DIV") {
            auto opNode = make_shared<TreeNode>("op", currentToken().value);
            match(currentToken().type); // Consume '/' or '*'
            opNode->children.push_back(node);
            opNode->children.push_back(factor());
            node = opNode; // Update the node with the operation subtree
        }
        return node;
    }

    shared_ptr<TreeNode> factor() {
        if (currentToken().type == "NUMBER") {
            auto node = make_shared<TreeNode>("Const", currentToken().value);
            match("NUMBER");
            return node;
        }
        if (currentToken().type == "IDENTIFIER") {
            auto node = make_shared<TreeNode>("id", currentToken().value);
            match("IDENTIFIER");
            return node;
        }
        if (currentToken().type == "OPENBRACKET") {
            advance();
            auto node = exp();
            match(" CLOSEDBRACKET");
            return node;
        }
        throw runtime_error("Error at line " + to_string(currentToken().line) + " : Invalid factor: " + currentToken().value);
    }

public:
    explicit Parser(const vector<Token> &tokens) : tokens(tokens), currentTokenIndex(0) {
    }

    shared_ptr<TreeNode> parse() {
        return program();
    }
};

void displayTree(const shared_ptr<TreeNode> &node, int depth = 0, bool isLast = true, const string &prefix = "") {
    if (!node) return;

    // Connector style for ASCII
    string connector = isLast ? "\\-- " : "|-- ";

    // Display node name and value (if value exists)
    cout << prefix << connector << node->name;
    if (!node->value.empty()) {
        cout << " (" << node->value << ")";
    }
    cout << endl;

    // Prepare prefix for child nodes
    string newPrefix = prefix + (isLast ? "    " : "|   ");

    // Recursively display children
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool lastChild = (i == node->children.size() - 1);
        displayTree(node->children[i], depth + 1, lastChild, newPrefix);
    }
}


int main() {
    string inputFile = R"(D:\Mohanad.Khh\ASU Senior 1 - Fall '23\Design Of Compilers\Input_File.txt)";
    string outputFile = R"(D:\Mohanad.Khh\ASU Senior 1 - Fall '23\Design Of Compilers\Output_File.txt)";


    ifstream inFile(inputFile);
    ofstream outFile(outputFile);

    try {
        if (!inFile.is_open()) {
            throw runtime_error("Error: Could not open input file.");
        }

        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open output file.");
        }

        int lineCount = 1;
        string line;
        vector<Token> outputTokens;

        // Read input file line by line
        while (getline(inFile, line)) {
            vector<Token> tokens = tokenize(line, lineCount);
            outputTokens.insert(outputTokens.end(), tokens.begin(), tokens.end());
            lineCount++;
        }

        // Write tokens to the output file
        for (const auto &token: outputTokens) {
            outFile << token.value << "," << token.type << endl;
        }

        // Parse tokens
        Parser parser(outputTokens);
        auto syntaxTree = parser.parse();

        // Display the syntax tree
        displayTree(syntaxTree);
    } catch (const runtime_error &e) {
        // Handle any runtime error by printing the message
        cerr << e.what() << endl;
        return 1; // Indicating failure
    }

    // Close files
    inFile.close();
    outFile.close();

    return 0; // Exit successfully
}
