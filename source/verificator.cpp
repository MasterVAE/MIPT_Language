#include <assert.h>
#include <stdio.h>

#include "verificator.h"
#include "tree.h" 

static TreeNode* VerifyNode(TreeNode* node);
static bool CheckOperation(TreeNode* node, Operation op);

#define SYNTAX                                                  \
{                                                               \
    printf("%s:%d Verificator error\n", __FILE__, __LINE__);    \
    node->correct = false;                                      \
}

Tree* Verify(Tree* tree)
{
    assert(tree);

    if(!tree->root) 
    {
        printf("%s:%d Verificator error\n", __FILE__, __LINE__);
    }

    VerifyNode(tree->root);

    return tree;
}

static bool CheckOperation(TreeNode* node, Operation op)
{
    return node && node->type == NODE_OPERATION && node->value.operation == op;
}

static TreeNode* VerifyNode(TreeNode* node)
{
    if(!node) return NULL;

    VerifyNode(node->left);
    VerifyNode(node->right);

    if( CheckOperation(node, OP_ASSIGN) 
    && !CheckOperation(node->left, OP_VARIABLE)) SYNTAX;

    if(CheckOperation(node, OP_WHILE)
    || CheckOperation(node, OP_IF)
    || CheckOperation(node, OP_OUT)
    || CheckOperation(node, OP_RETURN))
    {
        if(!CheckOperation(node->parent, OP_LINE)) SYNTAX;
    }

    if(CheckOperation(node, OP_FUNCTION))
    {
        if(node->left && node->left->type == NODE_IDENTIFICATOR
        && CheckOperation(node->right, OP_FUNCTION))
        {
            if(!CheckOperation(node->parent, OP_LINE)) SYNTAX;
        }
    }

    return node;
}