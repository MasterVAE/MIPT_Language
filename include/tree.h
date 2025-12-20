#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>

enum NodeType
{
    NODE_OPERATION,
    NODE_VARIABLE,
    NODE_CONSTANT
};

enum Operation
{
    OP_UNKNOWN,
    OP_ADD,
    OP_SUB,
    OP_ASSIGN,
    OP_WHILE,
    OP_IF,
    OP_BLOCK,
    OP_LINE,
    OP_EMPTY,
    OP_IN,
    OP_OUT,
    OP_EQUAL,
    OP_BIGGER
};

union NodeValue
{
    char variable;
    int constant;
    Operation operation;
};

struct TreeNode
{
    NodeType type;
    NodeValue value;

    TreeNode* left;
    TreeNode* right;

    TreeNode* parent;
};

struct Tree
{
    TreeNode* root; 
};

Tree* CreateTree();
void TreeDestroy(Tree* tree);
void NodeDestroy(TreeNode* node);
TreeNode* CreateNode(NodeType type, NodeValue value, 
                     TreeNode* left = NULL, TreeNode* right = NULL, TreeNode* parent = NULL);
TreeNode* CopyNode(TreeNode* node);
void SetParents(Tree* tree);

static const size_t BUFFER_SIZE = 128;

#endif // TREE_H_