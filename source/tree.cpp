#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "tree.h"

static void SetNodeParent(TreeNode* node, TreeNode* parent);

Tree* CreateTree()
{
    Tree* tree = (Tree*)calloc(1, sizeof(Tree));
    if(!tree) return NULL;

    tree->root = NULL;

    return tree;
}

void TreeDestroy(Tree* tree)
{
    if(!tree) return;

    NodeDestroy(tree->root);
    free(tree);
}

void NodeDestroy(TreeNode* node)
{
    if(!node) return;

    NodeDestroy(node->left);
    NodeDestroy(node->right);

    if(node->type == NODE_IDENTIFICATOR)
    {
        free(node->value.identificator);
    }

    free(node);
}

TreeNode* CreateNode(NodeType type, NodeValue value, 
                                    TreeNode* left, TreeNode* right, TreeNode* parent)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(!node) return NULL;

    node->type = type;
    node->value = value;

    node->correct = true;

    node->left = left;
    node->right = right;

    node->parent = parent;

    return node;
}

TreeNode* CopyNode(TreeNode* node)
{
    if(!node) return NULL;

    return CreateNode(node->type, node->value, CopyNode(node->left), 
                                               CopyNode(node->right));
}

void SetParents(Tree* tree)
{
    assert(tree);

    SetNodeParent(tree->root, NULL);
}

static void SetNodeParent(TreeNode* node, TreeNode* parent)
{
    if(!node) return;

    node->parent = parent;

    SetNodeParent(node->left, node);
    SetNodeParent(node->right, node);
}

