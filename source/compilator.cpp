#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "compilator.h"

#define PRINT(string1, string2) fprintf(file, string1, string2);

#define ERROR fprintf(stderr, "%s:%d Compilator error\n", __FILE__, __LINE__);

static Compilator* CreateCompilator();

static void DestroyCompilator(Compilator* compilator);

static void CompileNode(TreeNode* node, FILE* file, Compilator* compilator);

void CompileTree(Tree* tree, FILE* file)
{
    assert(tree);
    assert(file);

    Compilator* compilator = CreateCompilator();

    CompileNode(tree->root, file, compilator);

    PRINT("HLT\n", "");

    PRINT("LABEL EQUAL\n"
          "JE EQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL EQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n", "");

    
    PRINT("LABEL NEQUAL\n"
          "JNE NEQUAL_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL NEQUAL_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n", "");
        
    PRINT("LABEL SMALLER\n"
          "JB SMALLER_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL SMALLER_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n", "");

    PRINT("LABEL BIGGER\n"
          "JA BIGGER_TRUE\n"
          "PUSH 0\n"
          "RET\n"
          "LABEL BIGGER_TRUE\n"
          "PUSH 1\n"
          "RET\n"
          "\n", "");


    DestroyCompilator(compilator);
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
            char* name = node->left->value.identificator;

            bool found = false;
            for(size_t i = 0; i < compilator->nametable.name_count; i++)
            {
                if(compilator->nametable.names[i] 
                && !strcmp(name, compilator->nametable.names[i]))
                {
                    PRINT("PUSHR %s\n", REG_NAMES[i]);
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

            PRINT("ADD\n", "")
            return;
        }
        case OP_SUB:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("SUB\n", "")
            return;
        }
        case OP_MUL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("MUL\n", "")
            return;
        }
        case OP_DIV:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("DIV\n", "")
            return;
        }
        case OP_ASSIGN:
        {
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
                for(i = 0; i < compilator->nametable.name_count; i++)
                {
                    if(!compilator->nametable.names[i])
                    {
                        compilator->nametable.names[i] = name;
                        break;
                    }
                }
            }

            CompileNode(node->right, file, compilator);

            PRINT("POPR %s\n", REG_NAMES[i]);
            PRINT("PUSHR %s\n", REG_NAMES[i]);

            return;
        }
        case OP_OUT:
        {
            CompileNode(node->left, file, compilator);
            PRINT("OUT\n", "");

            return;
        }
        case OP_IN:
        {
            PRINT("IN\n", "");

            return;
        }
        case OP_IF:
        {
            size_t lable = compilator->label++;

            CompileNode(node->left, file, compilator);

            PRINT("PUSH 0\n", "");

            PRINT("JE %d\n", lable);

            CompileNode(node->right, file, compilator);

            PRINT("LABEL %lu\n", lable);

            return;
        }
        case OP_WHILE:
        {
            size_t label1 = compilator->label++;
            size_t label2 = compilator->label++;

            PRINT("LABEL %lu\n", label1);

            CompileNode(node->left, file, compilator);

            PRINT("PUSH 0\n", "");

            PRINT("JE %d\n", label2);

            CompileNode(node->right, file, compilator);

            PRINT("JMP %d\n", label1);

            PRINT("LABEL %lu\n", label2);

            return;
        }
        case OP_EQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL EQUAL\n", "");

            return;
        }
        case OP_NEQUAL:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL NEQUAL\n", "");

            return;
        }
        case OP_SMALLER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL SMALLER\n", "");

            return;
        }
        case OP_BIGGER:
        {
            CompileNode(node->left, file, compilator);
            CompileNode(node->right, file, compilator);

            PRINT("CALL BIGGER\n", "");

            return;
        }
    }
}

static Compilator* CreateCompilator()
{
    Compilator* comp = (Compilator*)calloc(1, sizeof(Compilator));

    comp->nametable.name_count = 8;
    comp->label = 0;

    return comp;
}

static void DestroyCompilator(Compilator* compilator)
{
    free(compilator);
}