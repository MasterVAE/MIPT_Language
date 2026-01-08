#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "compilator.h"

#define PRINT(...) fprintf(file, __VA_ARGS__);

#define ERROR   {                                                                       \
                    fprintf(stderr, "%s:%d Compilator error\n", __FILE__, __LINE__);    \
                    return;                                                             \
                }

static Compilator* CreateCompilator();

static void DestroyCompilator(Compilator* compilator);

static void CompileNode(TreeNode* node, FILE* file, Compilator* compilator);

static void CompileSystemCode(FILE* file);
static void CompileFunctions(FILE* file, Compilator* compilator);
static void CompileArguments(TreeNode* node, FILE* file, Compilator* compilator);

static bool CheckOperation(TreeNode* node, Operation op);
static size_t VariableCount(Tree* tree);

static size_t SearchVarInNametable(Compilator* compilator, const char* identificator);
static size_t SearchFuncInNametable(Compilator* compilator, const char* identificator);

struct VarCounter
{
    char** idents;
    size_t idents_count;
};

static void NodeVariableCount(TreeNode* node, VarCounter* counter);

void CompileTree(Tree* tree, FILE* file)
{
    assert(tree);
    assert(file);

    Compilator* compilator = CreateCompilator();
    if(!compilator) return;

    compilator->variable_count = VariableCount(tree);
    if(!compilator->variable_count)
    {
        DestroyCompilator(compilator);
        return;
    }

    CompileNode(tree->root, file, compilator);

    CompileSystemCode(file);

    CompileFunctions(file, compilator);

    DestroyCompilator(compilator);
}

static void CompileSystemCode(FILE* file)
{
    assert(file);

    PRINT("HLT                          #END OF MAIN PROG\n\n");

    PRINT("LABEL 0_EQUAL                #EQUAL CALL\n"
          "JE 0_EQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_EQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");
    
    PRINT("LABEL 0_NEQUAL               #NEQUAL CALL\n"
          "JNE 0_NEQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_NEQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");
        
    PRINT("LABEL 0_SMALLER              #SMALLER CALL\n"
          "JB 0_SMALLER_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_SMALLER_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");

    PRINT("LABEL 0_BIGGER               #BIGGER CALL\n"
          "JA 0_BIGGER_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_BIGGER_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");
}

static void CompileFunctions(FILE* file, Compilator* compilator)
{
    assert(file);
    assert(compilator);

    for(size_t i = 0; i < compilator->nametable.function_count; i++)
    {
        TreeNode* func = compilator->nametable.functions[i];
        PRINT("LABEL %s                     #FUNCTION %s\n", func->left->value.identificator,
                                                             func->left->value.identificator);

        CompileArguments(func->right->left, file, compilator);

        CompileNode(func->right->right, file, compilator);

        PRINT("PUSHR SR2\n");
        PRINT("PUSH 1\n");
        PRINT("SUB\n");
        PRINT("POPR SR2\n");
        PRINT("RET\n");
    }
}

static void CompileArguments(TreeNode* node, FILE* file, Compilator* compilator)
{
    assert(file);
    assert(compilator);

    if(!node) return;

    if(CheckOperation(node, OP_ARGUMENT))
    {
        CompileArguments(node->left, file, compilator);
        CompileArguments(node->right, file, compilator);
    }
    else if(CheckOperation(node, OP_VARIABLE))
    {
        const char* name = node->left->value.identificator;

        size_t i = SearchVarInNametable(compilator, name);
        
        if(i >= compilator->nametable.name_count)
        {
            compilator->nametable.name_count++;
            const char** new_names = (const char**)realloc(compilator->nametable.names, 
                                                sizeof(char*) * compilator->nametable.name_count);
            if(!new_names)
            {
                free(compilator->nametable.names);
                fprintf(stderr, "Memory error\n");

                return;
            }

            compilator->nametable.names = new_names;
            compilator->nametable.names[compilator->nametable.name_count - 1] = name;
        }

        PRINT("\nPUSH %lu                 #VARIABLE %s\n", i, name);
        PRINT("PUSHR SR2\n");
        PRINT("PUSH %lu\n", compilator->variable_count);
        PRINT("MUL\n");
        PRINT("ADD\n");
        PRINT("POPR SR1\n");
        PRINT("POPM [SR1]\n\n");
    }
    else
    {
        ERROR;
        return;
    }

}

static void CompileNode(TreeNode* node, FILE* file, Compilator* compilator)
{
    assert(node);
    assert(file);
    assert(compilator);

    assert(node->type != NODE_IDENTIFICATOR);

    if(node->type == NODE_CONSTANT)
    {
        PRINT("PUSH %d\n", node->value.constant);
        return;
    }

    switch (node->value.operation)
    {
        case OP_LINE:
        {
            if(node->left) CompileNode(node->left, file, compilator);
            if(node->right) CompileNode(node->right, file, compilator);
            return;
        }
        case OP_VARIABLE:
        {
            const char* name = node->left->value.identificator;

            size_t i = SearchVarInNametable(compilator, name);

            if(i >= compilator->nametable.name_count) ERROR;

            PRINT("\nPUSH %lu                 #VARIABLE %s\n", i, name);
            PRINT("PUSHR SR2\n");
            PRINT("PUSH %lu\n", compilator->variable_count);
            PRINT("MUL\n");
            PRINT("ADD\n");
            PRINT("POPR SR1\n");
            PRINT("PUSHM [SR1]\n\n");
                    
            return;
        }
        case OP_ADD:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("ADD\n")
            return;
        }
        case OP_SUB:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("SUB\n")
            return;
        }
        case OP_MUL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("MUL\n")
            return;
        }
        case OP_DIV:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("DIV\n")
            return;
        }
        case OP_ASSIGN:
        {
            if(!node->left || !node->left->left) ERROR;

            const char* name = node->left->left->value.identificator;

            size_t i = SearchVarInNametable(compilator, name);
    
            if(i == compilator->nametable.name_count)
            {
                compilator->nametable.name_count++;
                const char** new_names = (const char**)realloc(compilator->nametable.names, 
                                                    sizeof(char*) * compilator->nametable.name_count);
                if(!new_names)
                {
                    free(compilator->nametable.names);

                    ERROR;
                    return;
                }

                compilator->nametable.names = new_names;
                compilator->nametable.names[compilator->nametable.name_count - 1] = name;
            }

            CompileNode(node->right, file, compilator);

            PRINT("\nPUSH %lu                 #VARIABLE %s\n", i, name);
            PRINT("PUSHR SR2\n");
            PRINT("PUSH %lu\n", compilator->variable_count);
            PRINT("MUL\n");
            PRINT("ADD\n");
            PRINT("POPR SR1\n");
            PRINT("POPM [SR1]\n");
            PRINT("PUSHM [SR1]\n\n");

            return;
        }
        case OP_OUT:
        {
            CompileNode(node->left, file, compilator);
            PRINT("OUT\n");

            return;
        }
        case OP_IN:
        {
            PRINT("IN\n");

            return;
        }
        case OP_IF:
        {
            size_t lable = compilator->current_label++;

            CompileNode(node->left, file, compilator);

            PRINT("PUSH 0\n");

            PRINT("JE %lu\n", lable);

            CompileNode(node->right, file, compilator);

            PRINT("LABEL %lu\n", lable);

            return;
        }
        case OP_WHILE:
        {
            size_t label1 = compilator->current_label++;
            size_t label2 = compilator->current_label++;

            PRINT("LABEL %lu\n", label1);

            CompileNode(node->left, file, compilator);

            PRINT("PUSH 0\n");

            PRINT("JE %lu\n", label2);

            CompileNode(node->right, file, compilator);

            PRINT("JMP %lu\n", label1);

            PRINT("LABEL %lu\n", label2);

            return;
        }
        case OP_EQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL 0_EQUAL\n");

            return;
        }
        case OP_NEQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL 0_NEQUAL\n");

            return;
        }
        case OP_SMALLER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL 0_SMALLER\n");

            return;
        }
        case OP_BIGGER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL 0_BIGGER\n");

            return;
        }
        case OP_FUNCTION:
        {
            char* name = node->left->value.identificator;

            if(CheckOperation(node->right, OP_FUNCTION)
            && (!node->right->left || node->right->left->type != NODE_IDENTIFICATOR))
            {

                size_t i = SearchFuncInNametable(compilator, name);
                if(i < compilator->nametable.function_count) ERROR;

                compilator->nametable.function_count++;

                compilator->nametable.functions 
                = (TreeNode**)realloc(compilator->nametable.functions, 
                                      compilator->nametable.function_count * sizeof(TreeNode*));

                compilator->nametable.functions[compilator->nametable.function_count - 1] = node;

                return;
            }

            CompileNode(node->right, file, compilator);

            PRINT("PUSHR SR2\n");
            PRINT("PUSH 1\n");
            PRINT("ADD\n");
            PRINT("POPR SR2\n");
            PRINT("CALL %s\n", name);

            return;
        }
        case OP_ARGUMENT:
        {
            CompileNode(node->right, file, compilator);
            CompileNode(node->left, file, compilator);

            return;
        }
        case OP_RETURN:
        {
            if(node->left)  CompileNode(node->left, file, compilator);

            PRINT("PUSHR SR2\n");
            PRINT("PUSH 1\n");
            PRINT("SUB\n");
            PRINT("POPR SR2\n");
            PRINT("RET\n");

            return;
        }
        case OP_EMPTY:
            ERROR;
        case OP_COMMA:
            ERROR;
        case OP_BRACKET_OPEN:
            ERROR;
        case OP_BRACKET_CLOSE:
            ERROR;
        case OP_FBRACKET_OPEN:
            ERROR;
        case OP_FBRACKET_CLOSE:
            ERROR;
        default:
        {
            return;
        }
    }
}

static Compilator* CreateCompilator()
{
    Compilator* comp = (Compilator*)calloc(1, sizeof(Compilator));
    if(!comp) return NULL;

    comp->nametable.name_count = 0;
    comp->nametable.names = (const char**)calloc(1, sizeof(char*));
    if(!comp->nametable.names)
    {
        free(comp);
        return NULL;
    }
    comp->current_label = 0;
    comp->variable_count = 0;

    comp->nametable.function_count = 0;
    comp->nametable.functions = (TreeNode**)calloc(1, sizeof(TreeNode*));
    if(!comp->nametable.functions)
    {
        free(comp->nametable.names);
        free(comp);
        return NULL;
    }

    return comp;
}

static void DestroyCompilator(Compilator* compilator)
{
    if(!compilator) return;

    free(compilator->nametable.functions);
    free(compilator->nametable.names);
    free(compilator);
}

static bool CheckOperation(TreeNode* node, Operation op)
{
    return node && node->type == NODE_OPERATION && node->value.operation == op;
}

static size_t VariableCount(Tree* tree)
{
    VarCounter counter;
    counter.idents_count = 1;
    counter.idents = (char**)calloc(1, sizeof(TreeNode*));
    if(!counter.idents) return 0;

    NodeVariableCount(tree->root, &counter);

    free(counter.idents);

    return counter.idents_count;

}

static void NodeVariableCount(TreeNode* node, VarCounter* counter)
{
    assert(counter);

    if(!node) return;

    if(!counter->idents) return;

    if(CheckOperation(node, OP_VARIABLE))
    {
        char* identificator = node->left->value.identificator;

        bool added = false;

        for(size_t i = 0; i < counter->idents_count; i++)
        {
            if(!counter->idents[i])
            {
                counter->idents[i] = identificator;
                added = true;
                break;
            }
            else
            {
                if(!strcmp(counter->idents[i], identificator))
                {
                    added = true;
                }
            }
        }

        if(!added)
        {
            counter->idents_count++;
            char** new_idents = (char**)realloc(counter->idents, 
                                                counter->idents_count * sizeof(char*));

            if(!new_idents)
            {
                free(counter->idents);
                counter->idents = NULL;
                counter->idents_count = 0;
                return;
            }

            new_idents[counter->idents_count - 1] = identificator;
            counter->idents = new_idents;
        }
    }

    NodeVariableCount(node->left, counter);
    NodeVariableCount(node->right, counter);

    if(!counter->idents)
    {
        counter->idents_count = 0;
        return;
    }
}

static size_t SearchVarInNametable(Compilator* compilator, const char* identificator)
{
    assert(compilator);
    assert(identificator);

    size_t i = 0;
    for(i = 0; i < compilator->nametable.name_count; i++)
    {
        if(compilator->nametable.names[i] 
        && !strcmp(identificator, compilator->nametable.names[i]))
        {
            return i;
        }
    }

    return i;
}

static size_t SearchFuncInNametable(Compilator* compilator, const char* identificator)
{
    assert(compilator);
    assert(identificator);

    size_t i = 0;
    for(i = 0; i < compilator->nametable.function_count; i++)
    {
        if(compilator->nametable.functions[i] 
        && !strcmp(identificator, compilator->nametable.functions[i]->left->value.identificator))
        {
            return i;
        }
    }

    return i;
}