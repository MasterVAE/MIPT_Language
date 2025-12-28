#ifndef OP_DATA_H
#define OP_DATA_H

#include "tree.h"
#include "stdlib.h"

struct Oper
{
    Operation op;
    const char* op_name;
    const char* op_code;
};

static Oper OP_DATA[] = 
{
    {OP_ADD,            "+",        "+"},
    {OP_SUB,            "-",        "-"},
    {OP_MUL,            "*",        "*"},
    {OP_DIV,            "/",        "/"},
    {OP_BIGGER,         "BIGGER",   ">"},
    {OP_SMALLER,        "SMALLER",  "<"},
    {OP_EQUAL,          "EQUAL",    "=="},
    {OP_NEQUAL,         "NOT_EQUAL","!="},
    {OP_ASSIGN,         "ASSIGN",   "="},
    {OP_WHILE,          "WHILE",    "while"},
    {OP_IF,             "IF",       "if"},
    {OP_IN,             "IN",       "in"},
    {OP_OUT,            "OUT",      "out"},
    {OP_RETURN,         "RETURN",   "return"},
    {OP_BRACKET_OPEN,   "",         "("},
    {OP_BRACKET_CLOSE,  "",         ")"},
    {OP_FBRACKET_OPEN,  "",         "{"},
    {OP_FBRACKET_CLOSE, "",         "}"},
    {OP_COMMA,          "",         ","},
    {OP_LINE,           "LINE",     ";"},
    {OP_EMPTY,          "EMPTY",    NULL},
    {OP_VARIABLE,       "VARIABLE", NULL},
    {OP_FUNCTION,       "FUNCTION", NULL},
    {OP_ARGUMENT,       "ARGUMENT", NULL}
};

const size_t OP_COUNT = sizeof(OP_DATA)/sizeof(OP_DATA[0]);

const char SYSTEM_SYMBOLS[] = {'(', ')', '{', '}', ';', '<', '>', '!', '=', '-', '+', '*', '/', 
                               ',', ' ', '\0'};

const size_t SYSTEM_SYMBOLS_COUNT = sizeof(SYSTEM_SYMBOLS)/sizeof(SYSTEM_SYMBOLS[0]);

const char* GetOpName(Operation op);

bool IsSystem(char symbol);

#endif // OP_DATA_H