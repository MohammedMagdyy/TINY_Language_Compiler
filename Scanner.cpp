#include "Scanner.h"
#include <unordered_map>
#include <stdexcept>
#include <cctype>

using namespace std;

// Global variables
bool inComment = false;

// Token map
unordered_map<string, string> tokenMap = {
    {";", "SEMICOLON"}, {"if", "IF"}, {"then", "THEN"}, {"end", "END"}, {"repeat", "REPEAT"},
    {"until", "UNTIL"}, {":=", "ASSIGN"}, {"read", "READ"}, {"write", "WRITE"},
    {"<", "LESSTHAN"}, {"=", "EQUAL"}, {"+", "PLUS"}, {"-", "MINUS"},
    {"*", "MULT"}, {"/", "DIV"}, {"(", "OPENBRACKET"}, {")", "CLOSEDBRACKET"}, {"else", "ELSE"}
};

// Check if a string is a number
bool isNumber(const string &str) {
    for (char c: str) {
        if (!isdigit(c)) return false;
    }
    return !str.empty();
}

// Check if a string is an identifier (letters only)
bool isIdentifier(const string &str) {
    for (char c: str) {
        if (!isalpha(c)) return false;
    }
    return !str.empty();
}

// Tokenizer implementation
vector<Token> tokenize(const string &line, int lineCount) {
    vector<Token> tokens;
    string currentWord;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        // Handle comment start
        if (c == '{') {
            inComment = true;
            continue;
        }

        // Handle comment end
        if (c == '}') {
            inComment = false;
            continue;
        }

        // Skip characters inside comments
        if (inComment) {
            continue;
        }

        // Check for whitespace to finalize a token
        if (isspace(c)) {
            if (!currentWord.empty()) {
                if (tokenMap.contains(currentWord)) {
                    tokens.push_back({currentWord, tokenMap[currentWord], lineCount});
                } else if (isNumber(currentWord)) {
                    tokens.push_back({currentWord, "NUMBER", lineCount});
                } else if (isIdentifier(currentWord)) {
                    tokens.push_back({currentWord, "IDENTIFIER", lineCount});
                } else {
                    throw runtime_error(
                        "Error at line " + to_string(lineCount) + "  :  Unknown token \"" + currentWord + "\"");
                }
                currentWord.clear();
            }
            continue;
        }

        // Handle symbols and operators
        if (ispunct(c)) {
            if (!currentWord.empty()) {
                if (tokenMap.contains(currentWord)) {
                    tokens.push_back({currentWord, tokenMap[currentWord], lineCount});
                } else if (isNumber(currentWord)) {
                    tokens.push_back({currentWord, "NUMBER", lineCount});
                } else if (isIdentifier(currentWord)) {
                    tokens.push_back({currentWord, "IDENTIFIER", lineCount});
                } else {
                    throw runtime_error(
                        "Error at line " + to_string(lineCount) + "  :  Unknown token \"" + currentWord + "\"");
                }
                currentWord.clear();
            }

            if (c == '<') {
                tokens.push_back({string(1, c), "LESSTHAN", lineCount});
            } else if (c == ':' && i + 1 < line.size() && line[i + 1] == '=') {
                tokens.push_back({string(1, c) + line[i + 1], "ASSIGN", lineCount});
                ++i; // Skip the next character
            } else {
                string symbol(1, c);
                if (tokenMap.contains(symbol)) {
                    tokens.push_back({symbol, tokenMap[symbol], lineCount});
                } else {
                    throw runtime_error(
                        "Error at line " + to_string(lineCount) + " : Unknown token \"" + string(1, c) + "\"");
                }
            }
            continue;
        }

        // If part of a word, add to currentWord
        currentWord += c;
    }

    // Process the last word if there is one
    if (!currentWord.empty()) {
        if (tokenMap.contains(currentWord)) {
            tokens.push_back({currentWord, tokenMap[currentWord], lineCount});
        } else if (isNumber(currentWord)) {
            tokens.push_back({currentWord, "NUMBER", lineCount});
        } else if (isIdentifier(currentWord)) {
            tokens.push_back({currentWord, "IDENTIFIER", lineCount});
        } else {
            throw runtime_error("Error at line " + to_string(lineCount) + "  :  Unknown token \"" + currentWord + "\"");
        }
    }

    return tokens;
}
