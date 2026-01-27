#include "assert.h"

#include "tree.h"
#include "optimizator.h"

static void OptimizeNode(TreeNode* node);
static bool IsMathOperation(TreeNode* node);

Tree* OptimizeTree(Tree* tree)
{
    assert(tree);
    OptimizeNode(tree->root);
    return tree;
}

static void OptimizeNode(TreeNode* node)
{
    if(!node) return;
    OptimizeNode(node->left);
    OptimizeNode(node->right);

    // Dead Code Elimination
    if(CheckOperation(node, OP_LINE), CheckOperation(node->left, OP_RETURN))
    {
        NodeDestroy(node->right);
        node->right = NULL;
        return;
    }

    // Constant Convolution
    if(IsMathOperation(node) &&
       CheckType(node->left, NODE_CONSTANT) &&
       CheckType(node->right, NODE_CONSTANT))
    {
        int value = 0;
        if(CheckOperation(node, OP_ADD))
        {
            value = node->left->value.constant + node->right->value.constant;
        }
        else if(CheckOperation(node, OP_SUB))
        {
            value = node->left->value.constant - node->right->value.constant;
        }
        else if(CheckOperation(node, OP_MUL))
        {
            value = node->left->value.constant * node->right->value.constant;
        }
        else if(CheckOperation(node, OP_DIV))
        {
            value = node->left->value.constant / node->right->value.constant;
        }

        TreeNode* parent = node->parent;

        bool right = parent->right == node;

        NodeDestroy(node);

        if(right)
        {
            parent->right = CreateNode(NODE_CONSTANT, NodeValue {.constant = value});
        }
        else
        {
            parent->left = CreateNode(NODE_CONSTANT, NodeValue {.constant = value});
        }

        return;
    }
}

static bool IsMathOperation(TreeNode* node)
{
    return CheckType(node, NODE_OPERATION) && (node->value.operation == OP_ADD || 
                                               node->value.operation == OP_SUB ||
                                               node->value.operation == OP_MUL ||
                                               node->value.operation == OP_DIV);
}