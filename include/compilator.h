#ifndef COMPILATOR_H
#define COMPILATOR_H

#include <stdio.h>

#include "tree.h"

static const char* REG_NAMES[] = 
{
    "SR1",
    "SR2",
    "SR3",
    "SR4",
    "SR5",
    "SR6",
    "SRX",
    "SRY"
};

struct Nametable
{
    char* names[8];
    size_t name_count = 8;
};

struct Compilator
{
    Nametable nametable;
    size_t label = 0;
};

void CompileTree(Tree* tree, FILE* file);

#endif // COMPILATOR_H