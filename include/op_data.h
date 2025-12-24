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
    {OP_ADD,        "+"},
    {OP_SUB,        "-"},
    {OP_MUL,        "*"},
    {OP_DIV,        "/"},
    {OP_ASSIGN,     "ASSIGN"},
    {OP_WHILE,      "WHILE"},
    {OP_IF,         "IF"},
    {OP_LINE,       "LINE"},
    {OP_EMPTY,      "EMPTY"},
    {OP_VARIABLE,   "VARIABLE"},
    {OP_FUNCTION,   "FUNCTION"},
    {OP_ARGUMENT,   "ARGUMENT"},
    {OP_IN,         "IN"},
    {OP_OUT,        "OUT"},
    {OP_BIGGER,     "BIGGER"},
    {OP_SMALLER,    "SMALLER"},
    {OP_EQUAL,      "EQUAL"},
    {OP_NEQUAL,     "NOT_EQUAL"},
    {OP_RETURN,     "RETURN"}
};

const size_t OP_COUNT = sizeof(OP_DATA)/sizeof(OP_DATA[0]);

const char SYSTEM_SYMBOLS[] = {'(', ')', '{', '}', ';', '<', '>', '!', '=', '-', '+', '*', '/', ',', '\0'};

const size_t SYSTEM_SYMBOLS_COUNT = sizeof(SYSTEM_SYMBOLS)/sizeof(SYSTEM_SYMBOLS[0]);

const char* GetOpName(Operation op);

bool IsSystem(char symbol);

#endif // OP_DATA_H