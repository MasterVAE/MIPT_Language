#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "read.h"

static size_t FileLength(FILE* file);

static TreeNode* Programm(char** programm);
static TreeNode* Argument(char** programm);
static TreeNode* Block(char** programm);
static TreeNode* Line(char** programm);
static TreeNode* Variable(char** programm);
static TreeNode* Constant(char** programm);
static TreeNode* Expression(char** programm);
static TreeNode* Function(char** programm);
static TreeNode* Term(char** programm);


#define SYNTAX                                                  \
{                                                               \
    printf("%d Syntax error\n%s\n", __LINE__, *programm);       \
    return NULL;                                                \
}

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

    printf("NEW_BUFFER \n%s\n", new_buffer);

    free(buffer);

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

    TreeNode* node = Block(programm);

    if(**programm != '\0') SYNTAX;

    return node;
}

static TreeNode* Block(char** programm)
{
    assert(programm);
    assert(*programm);

    if(**programm != '{') SYNTAX;

    (*programm)++;

    TreeNode* node = Line(programm);

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

    TreeNode* node = NULL;

    // TODO читать в Expression (=/</> -> +/- -> func/var/const)
    node = Expression(programm);

    if(**programm != ';') SYNTAX;

    (*programm)++;

    if(node) return node;

    return CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EMPTY});
}

static TreeNode* Expression(char** programm)
{
    assert(programm);
    assert(*programm);

    printf("EXP \n%s\n", *programm);

    TreeNode* node = NULL;

    node = Term(programm);
    if(!node) return NULL;

    while(1)
    {
        if(!strncmp(*programm, "==", 2))
        {
            (*programm)+=2;
            TreeNode* new_node = Term(programm);
            if(!new_node) return NULL;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EQUAL}, node, new_node);
        }
        else if(**programm == '=')
        {
            (*programm)++;
            TreeNode* new_node = Term(programm);
            if(!new_node) return NULL;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_ASSIGN}, node, new_node);
        }
        else if(**programm == '>')
        {
            (*programm)++;
            TreeNode* new_node = Term(programm);
            if(!new_node) return NULL;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_BIGGER}, node, new_node);
        }
        else if(**programm == '<')
        {
            (*programm)++;
            TreeNode* new_node = Term(programm);
            if(!new_node) return NULL;

            node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_BIGGER}, new_node, node);
        }
        else break;
    }

    return node;
}

static TreeNode* Term(char** programm)
{
    assert(programm);
    assert(*programm);

    printf("TERM \n%s\n", *programm);

    TreeNode* node = NULL;

    node = Constant(programm);

    if(!node) node = Function(programm);

    if(!node) node = Variable(programm);

    return node;
}

static TreeNode* Function(char** programm)
{
    assert(programm);
    assert(*programm);

    TreeNode* node = NULL;

    if(!strncmp(*programm, "in", 2))
    {
        (*programm) += 2;
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_IN}, Argument(programm));
    }

    if(!strncmp(*programm, "out", 3))
    {
        (*programm) += 3;
        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_OUT}, Argument(programm));
    }
    if(!strncmp(*programm, "add", 3))
    {
        (*programm) += 3;

        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Argument(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_ADD}, node1, node2);
    }
    if(!strncmp(*programm, "sub", 3))
    {
        (*programm) += 3;
        
        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Argument(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_SUB}, node1, node2);
    }
    if(!strncmp(*programm, "if", 2))
    {
        (*programm) += 2;

        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Block(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_IF}, node1, node2);

    }
    if(!strncmp(*programm, "while", 5))
    {
        (*programm) += 5;
        
        TreeNode* node1 = Argument(programm);
        TreeNode* node2 = Block(programm);

        node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_WHILE}, node1, node2);
    }

    return node;
}

static TreeNode* Argument(char** programm)
{
    TreeNode* node = NULL;

    if(**programm != '(') SYNTAX;

    (*programm)++;

    node =Expression(programm);

    if(**programm != ')') SYNTAX;

    (*programm)++;

    if(!node) node = CreateNode(NODE_OPERATION, NodeValue {.operation = OP_EMPTY});

    return node;
}

static TreeNode* Constant(char** programm)
{
    int value = 0;
    int count = 0;
    sscanf(*programm, "%d%n", &value, &count);

    if(count <= 0) return NULL;

    *programm += count;

    return CreateNode(NODE_CONSTANT, NodeValue {.constant = value});
}

static TreeNode* Variable(char** programm)
{
    assert(programm);
    assert(*programm);

    TreeNode* node = NULL;

    if(**programm != '\0' && **programm != '}'  && **programm != '{'  && **programm != '(' && **programm != ')' && **programm != ';')  
    {
        node = CreateNode(NODE_VARIABLE, NodeValue {.variable = **programm});
        (*programm)++;
    }

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