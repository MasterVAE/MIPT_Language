#ifndef OP_DATA_H
#define OP_DATA_H

#include "tree.h"
#include "stdlib.h"

struct Oper
{
    Operation op;
    const char* op_name;
};

static Oper OP_DATA[] = 
{
    {OP_ADD, "OP_ADD"},
    {OP_SUB, "OP_SUB"},
    {OP_ASSIGN, "OP_ASSIGN"},
    {OP_WHILE, "OP_WHILE"},
    {OP_IF, "OP_IF"},
    {OP_BLOCK, "OP_BLOCK"},
    {OP_LINE, "OP_LINE"},
    {OP_EMPTY, "OP_EMPTY"},
    {OP_IN, "OP_IN"},
    {OP_OUT, "OP_OUT"},
    {OP_BIGGER, "OP_BIGGER"},
    {OP_EQUAL, "OP_EQUAL"}
};

const size_t OP_COUNT = sizeof(OP_DATA)/sizeof(OP_DATA[0]);

const char* GetOpName(Operation op);

#endif // OP_DATA_H