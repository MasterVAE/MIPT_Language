#ifndef TOKENIZATOR_H
#define TOKENIZATOR_H

#include <stdlib.h>

#include "tree.h"

struct Token
{
    char* line;
    NodeType type;
    NodeValue value;
};

struct Program
{
    Token* tokens;
    size_t token_count; 
};

Program* Tokenize(const char* filename);

#endif // TOKENIZATOR_H