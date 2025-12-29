#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "compilator.h"

#define PRINT1(string1) fprintf(file, string1);
#define PRINT2(string1, string2) fprintf(file, string1, string2);

#define ERROR fprintf(stderr, "%s:%d Compilator error\n", __FILE__, __LINE__);

static Compilator* CreateCompilator();

static void DestroyCompilator(Compilator* compilator);

static void CompileNode(TreeNode* node, FILE* file, Compilator* compilator);

static void CompileSystemCode(FILE* file);
static void CompileFunctions(FILE* file, Compilator* compilator);
static void CompileArguments(TreeNode* node, FILE* file, Compilator* compilator);

void CompileTree(Tree* tree, FILE* file)
{
    assert(tree);
    assert(file);

    Compilator* compilator = CreateCompilator();
    if(!compilator) return;

    CompileNode(tree->root, file, compilator);

    CompileSystemCode(file);

    CompileFunctions(file, compilator);

    DestroyCompilator(compilator);
}

static void CompileSystemCode(FILE* file)
{
    PRINT1("HLT\n\n");

    PRINT1("LABEL 0_EQUAL\n"
          "JE 0_EQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_EQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");
    
    PRINT1("LABEL 0_NEQUAL\n"
          "JNE 0_NEQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_NEQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");
        
    PRINT1("LABEL 0_SMALLER\n"
          "JB 0_SMALLER_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL 0_SMALLER_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n");

    PRINT1("LABEL 0_BIGGER\n"
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
        PRINT2("LABEL %s\n", func->left->value.identificator);

        CompileArguments(func->right->left, file, compilator);

        CompileNode(func->right->right, file, compilator);

        PRINT1("RET\n");
    }
}

static void CompileArguments(TreeNode* node, FILE* file, Compilator* compilator)
{
    assert(file);
    assert(compilator);

    if(!node) return;

    if(node->type == NODE_OPERATION && node->value.operation == OP_ARGUMENT)
    {
        CompileArguments(node->left, file, compilator);
        CompileArguments(node->right, file, compilator);
    }
    else if(node->type == NODE_OPERATION && node->value.operation == OP_VARIABLE)
    {
        char* name = node->left->value.identificator;

        size_t i = 0;
        for(i = 0; i < compilator->nametable.name_count; i++)
        {
            if(compilator->nametable.names[i] 
            && !strcmp(name, compilator->nametable.names[i]))
            {
                break;
            }
        }
        if(i == compilator->nametable.name_count)
        {
            compilator->nametable.name_count++;
            char** new_names = (char**)realloc(compilator->nametable.names, 
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

        PRINT2("POPM %lu\n", i);
    }
    else
    {
        printf("%p\n", node);
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
        PRINT2("PUSH %d\n", node->value.constant);
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
            char* name = node->left->value.identificator;

            bool found = false;
            for(size_t i = 0; i < compilator->nametable.name_count; i++)
            {
                if(compilator->nametable.names[i] 
                && !strcmp(name, compilator->nametable.names[i]))
                {
                    PRINT2("PUSHM %lu\n", i);
                    found = true;
                    break;
                }
            }

            if(!found) ERROR;

            return;
        }
        case OP_ADD:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("ADD\n")
            return;
        }
        case OP_SUB:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("SUB\n")
            return;
        }
        case OP_MUL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("MUL\n")
            return;
        }
        case OP_DIV:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("DIV\n")
            return;
        }
        case OP_ASSIGN:
        {
            if(!node->left || !node->left->left) ERROR;

            char* name = node->left->left->value.identificator;

            size_t i = 0;
            for(i = 0; i < compilator->nametable.name_count; i++)
            {
                if(compilator->nametable.names[i] 
                && !strcmp(name, compilator->nametable.names[i]))
                {
                    break;
                }
            }
            if(i == compilator->nametable.name_count)
            {
                compilator->nametable.name_count++;
                char** new_names = (char**)realloc(compilator->nametable.names, 
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

            PRINT2("POPM %lu\n", i);
            PRINT2("PUSHM %lu\n", i);

            return;
        }
        case OP_OUT:
        {
            CompileNode(node->left, file, compilator);
            PRINT1("OUT\n");

            return;
        }
        case OP_IN:
        {
            PRINT1("IN\n");

            return;
        }
        case OP_IF:
        {
            size_t lable = compilator->label++;

            CompileNode(node->left, file, compilator);

            PRINT1("PUSH 0\n");

            PRINT2("JE %lu\n", lable);

            CompileNode(node->right, file, compilator);

            PRINT2("LABEL %lu\n", lable);

            return;
        }
        case OP_WHILE:
        {
            size_t label1 = compilator->label++;
            size_t label2 = compilator->label++;

            PRINT2("LABEL %lu\n", label1);

            CompileNode(node->left, file, compilator);

            PRINT1("PUSH 0\n");

            PRINT2("JE %lu\n", label2);

            CompileNode(node->right, file, compilator);

            PRINT2("JMP %lu\n", label1);

            PRINT2("LABEL %lu\n", label2);

            return;
        }
        case OP_EQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("CALL 0_EQUAL\n");

            return;
        }
        case OP_NEQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("CALL 0_NEQUAL\n");

            return;
        }
        case OP_SMALLER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("CALL 0_SMALLER\n");

            return;
        }
        case OP_BIGGER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT1("CALL 0_BIGGER\n");

            return;
        }
        case OP_FUNCTION:
        {
            char* name = node->left->value.identificator;

            if(node->right->type == NODE_OPERATION && node->right->value.operation == OP_FUNCTION
            && (!node->right->left || node->right->left->type != NODE_IDENTIFICATOR))
            {
                for(size_t i = 0; i < compilator->nametable.function_count; i++)
                {
                    if(!strcmp(name, compilator->nametable.functions[i]->left->value.identificator))
                    {
                        ERROR;
                        break;
                    }
                }

                compilator->nametable.function_count++;

                compilator->nametable.functions 
                = (TreeNode**)realloc(compilator->nametable.functions, 
                    compilator->nametable.function_count * sizeof(TreeNode*));

                compilator->nametable.functions[compilator->nametable.function_count - 1] = node;

                return;
            }

            CompileNode(node->right, file, compilator);

            PRINT2("CALL %s\n", name);

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

            PRINT1("RET\n");

            return;
        }
        case OP_EMPTY:
        {
            return;
        }
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
    comp->nametable.names = (char**)calloc(1, sizeof(char*));
    if(!comp->nametable.names)
    {
        free(comp);
        return NULL;
    }
    comp->label = 0;

    comp->nametable.function_count = 0;
    comp->nametable.functions = (TreeNode**)calloc(1, sizeof(TreeNode*));

    return comp;
}

static void DestroyCompilator(Compilator* compilator)
{
    free(compilator->nametable.functions);
    free(compilator->nametable.names);
    free(compilator);
}