#ifndef OP_DATA_H
#define OP_DATA_H

#include "tree.h"
#include "stdlib.h"

struct Oper
{
    Operation op;
    const char* op_name;
    const char* op_code;
    const char* color;
};

static Oper OP_DATA[] = 
{
    {OP_ADD,            "+",        "+",        "goldenrod3"},
    {OP_SUB,            "-",        "-",        "goldenrod3"},
    {OP_MUL,            "*",        "*",        "goldenrod3"},
    {OP_DIV,            "/",        "/",        "goldenrod3"},
    {OP_BIGGER,         "BIGGER",   ">",        "gray51"},
    {OP_SMALLER,        "SMALLER",  "<",        "gray51"},
    {OP_EQUAL,          "EQUAL",    "==",       "gray51"},
    {OP_NEQUAL,         "NOT_EQUAL","!=",       "gray51"},
    {OP_ASSIGN,         "ASSIGN",   "=",        "darkolivegreen3"},
    {OP_WHILE,          "WHILE",    "while",    "darkolivegreen3"},
    {OP_IF,             "IF",       "if",       "darkolivegreen3"},
    {OP_IN,             "IN",       "in",       "darkolivegreen3"},
    {OP_OUT,            "OUT",      "out",      "darkolivegreen3"},
    {OP_RETURN,         "RETURN",   "return",   "darkolivegreen3"},
    {OP_BRACKET_OPEN,   NULL,         "("},
    {OP_BRACKET_CLOSE,  NULL,         ")"},
    {OP_FBRACKET_OPEN,  NULL,         "{"},
    {OP_FBRACKET_CLOSE, NULL,         "}"},
    {OP_COMMA,          NULL,         ","},
    {OP_LINE,           "LINE",     ";",        "cyan3"},
    {OP_EMPTY,          "EMPTY",    NULL,       "cyan3"},
    {OP_VARIABLE,       "VARIABLE", NULL,       "cyan3"},
    {OP_FUNCTION,       "FUNCTION", NULL,       "cyan3"},
    {OP_ARGUMENT,       "ARGUMENT", NULL,       "cyan3"}
};

const size_t OP_COUNT = sizeof(OP_DATA)/sizeof(OP_DATA[0]);

const char SYSTEM_SYMBOLS[] = {'(', ')', '{', '}', ';', '<', '>', '!', '=', '-', '+', '*', '/', 
                               ',', ' ', '\0'};

const size_t SYSTEM_SYMBOLS_COUNT = sizeof(SYSTEM_SYMBOLS)/sizeof(SYSTEM_SYMBOLS[0]);

const char* GetOpName(Operation op);
const char* GetOpColor(Operation operation);

bool IsSystem(char symbol);

#endif // OP_DATA_H