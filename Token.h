#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

struct Token {
    string value;
    string type;
    int line;
};

#endif // TOKEN_H
