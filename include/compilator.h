#ifndef COMPILATOR_H
#define COMPILATOR_H

#include <stdio.h>

#include "tree.h"

struct Nametable
{
    char** names;
    size_t name_count;

    TreeNode** functions;
    size_t function_count;
};

struct Compilator
{
    Nametable nametable;
    size_t label = 0;
};

void CompileTree(Tree* tree, FILE* file);

#endif // COMPILATOR_H