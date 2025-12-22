#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "read.h"
#include "op_data.h"

// #define DEBUG_

static size_t FileLength(FILE* file);

static TreeNode* Programm   (char** programm);
static TreeNode* Block      (char** programm);
static TreeNode* Line       (char** programm);
static TreeNode* Assigment  (char** programm);
static TreeNode* Expression (char** programm);
static TreeNode* Term       (char** programm);
static TreeNode* Primar     (char** programm);
static TreeNode* Element    (char** programm);
static TreeNode* Brackets   (char** programm);
static TreeNode* Constant   (char** programm);
static TreeNode* FuncVar    (char** programm);
static TreeNode* Identif    (char** programm);
static TreeNode* System     (char** programm);
static TreeNode* Argument   (char** programm);

#define SYNTAX                                                  \
{                                                               \
    printf("%s:%d Syntax error\n%s\n", __FILE__, __LINE__, *programm);       \
    return NULL;                                                \
}

#ifdef DEBUG_
    #define DEBUG_PRINT(func)                                       \
    {                                                               \      
        printf(func "\n%s\n", *programm);                           \
    }   
#else
    #define DEBUG_PRINT(func)
#endif

Tree* ReadProgramm(const char* filename)
{
    assert(filename);

    FILE* file = fopen(filename, "r");
    if(!file) return NULL;

    size_t lenght = FileLength(file);
    char* buffer = (char*)calloc(lenght + 1, sizeof(char));
    if(!buffer) return NULL;

    fread(buffer, lenght, 1, file);
    buffer[lenght] = '\0';

    char* new_buffer = (char*)calloc(lenght + 1, sizeof(char));
    if(!new_buffer)
    {   
        free(buffer);
        return NULL;
    }

    size_t buffer_i = 0;
    size_t new_buffer_i = 0;

    while(buffer[buffer_i] != '\0')
    {
        if(!isspace(buffer[buffer_i]))
        {
            new_buffer[new_buffer_i++] = buffer[buffer_i];
        }
        buffer_i++;
    }

    free(buffer);

    printf("BUFFER \n%s\n", new_buffer);

    char* buffer_copy = new_buffer;

    Tree* tree = CreateTree();

    tree->root = Programm(&buffer_copy);

    free(new_buffer);
    fclose(file);
    return tree;
}

static TreeNode* Programm(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("PROGRAMM");

    TreeNode* node = Block(programm);

    if(**programm != '\0') SYNTAX;

    return node;
}

static TreeNode* Block(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("BLOCK");

    if(**programm != '{') SYNTAX;

    (*programm)++;

    TreeNode* node = Line(programm);

    if(node && **programm == '}')
    {
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_LINE}, 
                                                     node);
    }

    if(!node && **programm == '}')
    {
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EMPTY});
    }

    while(**programm != '}')
    {
        TreeNode* new_line = Line(programm);
        if(!new_line) return NULL;

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_LINE}, 
                                                     node, new_line);
    }

    (*programm)++;

    return node;
}

static TreeNode* Line(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("LINE");

    TreeNode* node = Assigment(programm);
    if(!node) return NULL;

    if(**programm != ';') SYNTAX;

    (*programm)++;

    if(node) return node;

    return CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EMPTY});
}

static TreeNode* Assigment(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("ASSIGMENT");

    TreeNode* node = Expression(programm);
    if(!node) return NULL;

    while(**programm == '=')
    {
        (*programm)++;

        TreeNode* new_node = Assigment(programm);
        if(!new_node) SYNTAX;

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_ASSIGN}, node, new_node);
    }

    return node;
}

static TreeNode* Expression(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("EXPRESSION");

    TreeNode* node = Term(programm);
    if(!node) return NULL;

    while(1)
    {
        if(!strncmp(*programm, "==", 2))
        {
            (*programm)+=2;
            TreeNode* new_node = Term(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EQUAL}, node, new_node);
        }
        else if(!strncmp(*programm, "!=", 2))
        {
            (*programm)+=2;
            TreeNode* new_node = Term(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_NEQUAL}, node, new_node);
        }
        else if(**programm == '>')
        {
            (*programm)++;
            TreeNode* new_node = Term(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_BIGGER}, node, new_node);
        }
        else if(**programm == '<')
        {
            (*programm)++;
            TreeNode* new_node = Term(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_SMALLER}, node, new_node);
        }
        else break;
    }

    return node;
}

static TreeNode* Term(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("TERM");

    TreeNode* node = Primar(programm);
    if(!node) return NULL;

    while(1)
    {
        if(**programm == '+')
        {
            (*programm)++;

            TreeNode* new_node = Primar(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_ADD}, node, new_node);
        }
        else if(**programm == '-')
        {
           (*programm)++;

            TreeNode* new_node = Primar(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_SUB}, node, new_node);
        }
        else break;
    }

    return node;
}

static TreeNode* Primar(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("PRIMAR");

    TreeNode* node = Element(programm);
    if(!node) return NULL;

    while(1)
    {
        if(**programm == '*')
        {
            (*programm)++;

            TreeNode* new_node = Element(programm);
            if(!new_node) SYNTAX;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_MUL}, node, new_node);
        }
        else if(**programm == '/')
        {
           (*programm)++;

            TreeNode* new_node = Element(programm);
            if(!new_node) SYNTAX;
            
            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_DIV}, node, new_node);
        }
        else break;
    }

    return node;
}


static TreeNode* Element(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("ELEMENT");

    TreeNode* node = NULL;

    node = Brackets(programm);
    if(!node) node = Constant(programm);
    if(!node) node = System(programm);
    if(!node) node = FuncVar(programm);

    return node;
}

static TreeNode* Brackets(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("BRACKETS");

    TreeNode* node = NULL;

    if(**programm != '(') return NULL;

    (*programm)++;

    node = Assigment(programm);

    if(**programm != ')') SYNTAX;

    (*programm)++;

    return node;
}

static TreeNode* Constant(char** programm)
{
    assert(programm);
    assert(*programm);
    
    DEBUG_PRINT("CONSTANT");

    int value = 0;
    int count = 0;
    sscanf(*programm, "%d%n", &value, &count);

    if(count <= 0) return NULL;

    *programm += count;

    return CreateNode(NODE_CONSTANT, NodeValue {.constant = value});
}

static TreeNode* System(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("SYSTEM");

    TreeNode* node = NULL;

    if(!strncmp(*programm, "in", 2))
    {
        (*programm) += 2;
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_IN});
    }
    else if(!strncmp(*programm, "out", 3))
    {
        (*programm) += 3;
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_OUT}, Argument(programm));
    }
    else if(!strncmp(*programm, "if", 2))
    {
        (*programm) += 2;

        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Block(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_IF}, node1, node2);

    }
    else if(!strncmp(*programm, "while", 5))
    {
        (*programm) += 5;
        
        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Block(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_WHILE}, node1, node2);
    }
    else if(!strncmp(*programm, "func", 4))
    {
        (*programm) += 4;
        
        TreeNode* node1 = Identif(programm);
        if(!node1) SYNTAX;

        TreeNode* node2 = Argument(programm);
        TreeNode* node3 = Block(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_FUNCTION}, node1, 
               CreateNode(NODE_OPERATION, NodeValue {.operation = OP_FUNCTION}, node2, node3));
    }
    else if(!strncmp(*programm, "return", 6))
    {
        (*programm) += 6;
        
        TreeNode* node1 = Assigment(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_RETURN}, node1);
    }

    return node;
}

static TreeNode* FuncVar(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("FUNCTION");

    TreeNode* ident = Identif(programm);
    if(!ident) return NULL;

    if(**programm == '(')
    {
        TreeNode* arg = Argument(programm);
        if(!arg) SYNTAX;
        return CreateNode(NODE_OPERATION, NodeValue {.operation = OP_FUNCTION}, ident, arg);
    }

    return CreateNode(NODE_OPERATION, NodeValue {.operation = OP_VARIABLE}, ident);

}

static TreeNode* Identif(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("IDENTIFICATOR");

    TreeNode* node = NULL;

    size_t len = 0;
    char* start = *programm;
    while(!IsSystem(**programm))
    {
        len += 1;
        (*programm)++;
    }

    if(len > 0)
    {
        node = CreateNode(NODE_IDENTIFICATOR, NodeValue {.identificator = strndup(start, len)});
    }

    return node;
}

static TreeNode* Argument(char** programm)
{
    assert(programm);
    assert(*programm);

    DEBUG_PRINT("ARGUMENT");

    TreeNode* node = NULL;

    if(**programm != '(') SYNTAX;

    (*programm)++;

    node = Assigment(programm);

    while(**programm == ',')
    {
        (*programm)++;
        TreeNode* node1 = Assigment(programm);
        if(!node1) SYNTAX;

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_ARGUMENT}, node, node1);
    }

    if(**programm != ')') SYNTAX;

    (*programm)++;

    if(!node) node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EMPTY});

    return node;
}


static size_t FileLength(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t filesize = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    return filesize;
}