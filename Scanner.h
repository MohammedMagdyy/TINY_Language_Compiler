#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include "Token.h"

using namespace std;

vector<Token> tokenize(const string& line, int lineCount);

#endif // SCANNER_H
