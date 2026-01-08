#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "tree.h"
#include "op_data.h"
#include "library.h"

static void SetNodeParent(TreeNode* node, TreeNode* parent);

static void SaveNode(TreeNode* node, FILE* file, size_t recursion);
TreeNode* LoadNode(char** buffer);

const size_t identificator_lenght = 100;

#define SKIP *buffer = SkipSpaces(*buffer);

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

void SaveTree(Tree* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* file = fopen(filename, "w+");
    if(!file)
    {
        fprintf(stderr, "ERROR opening file %s", filename);
        return;
    }

    SaveNode(tree->root, file, 1);

    fclose(file);
}

static void SaveNode(TreeNode* node, FILE* file, size_t recursion)
{   
    assert(file);

    if(!node) return;

    if(node->type == NODE_IDENTIFICATOR)
    {
        fprintf(file, "%s", node->value.identificator);
    }
    else if(node->type == NODE_CONSTANT)
    {
        fprintf(file, "%i", node->value.constant);
    }
    else
    {
        fprintf(file, "%s", GetOpName(node->value.operation));
    }

    fprintf(file, "\n");
    for(size_t i = 0; i < recursion; i++)
    {
        fprintf(file, "\t");
    }
    fprintf(file, "(");

    SaveNode(node->left, file, recursion + 1);

    fprintf(file, "\n");
    for(size_t i = 0; i < recursion; i++)
    {
        fprintf(file, "\t");
    }
    fprintf(file, "|");

    SaveNode(node->right, file, recursion + 1);

    fprintf(file, ")");

}

Tree* LoadTree(const char* filename)
{
    assert(filename);

    char* buffer = ReadFile(filename);
    if(!buffer) return NULL;

    Tree* tree = CreateTree();
    if(!tree)
    {
        free(buffer);
        fprintf(stderr, "ERROR creating tree\n");
        return NULL;
    }

    char* buffer_copy = buffer;
    tree->root = LoadNode(&buffer_copy);

    free(buffer);
    return tree;
}

TreeNode* LoadNode(char** buffer)
{
    assert(buffer);
    assert(*buffer);

    TreeNode* node = NULL;

    SKIP;

    int value = 0;
    int count = 0;
    sscanf(*buffer, "%d%n", &value, &count);

    (*buffer) += count;

    bool found = false;

    if(count > 0)
    {
        found = true;

        node = CreateNode(NODE_CONSTANT, NodeValue {.constant = value}); 
        if(!node)
        {
            fprintf(stderr, "ERROR reading tree 1\n");
            return NULL;
        }      
    }
    if(!found)
    {
        for(size_t i = 0; i < OP_COUNT; i++)
        {
            Oper data = OP_DATA[i];
            if(!data.op_name) continue;

            if(!strncmp(*buffer, data.op_name, strlen(data.op_name)))
            {
                node = CreateNode(NODE_OPERATION, NodeValue {.operation = data.op});
                if(!node)
                {
                    fprintf(stderr, "ERROR reading tree 2\n");
                    return NULL;
                }   
                found = true;
                (*buffer) += strlen(data.op_name);

                break;
            }
        }
    }
    if(!found)
    {
        char ident[identificator_lenght] = "";
        if(!sscanf(*buffer, "%[^(|)\n ]", ident)) return NULL;

        node = CreateNode(NODE_IDENTIFICATOR, NodeValue {.identificator = strdup(ident)});
        if(!node)
        {
            fprintf(stderr, "ERROR reading tree 4\n");
            return NULL;
        } 

        (*buffer) += strlen(ident);
    }

    SKIP;

    if(**buffer != '(')
    {
        fprintf(stderr, "ERROR reading tree 5 %s\n", *buffer);
        return NULL;
    } 

    (*buffer)++;

    TreeNode* node_left = LoadNode(buffer);

    SKIP;

    if(**buffer != '|')
    {
        fprintf(stderr, "ERROR reading tree 6\n");
        return NULL;
    } 

    (*buffer)++;

    TreeNode* node_right = LoadNode(buffer);

    SKIP;

    if(**buffer != ')')
    {
        fprintf(stderr, "ERROR reading tree 7 %s\n", *buffer);
        return NULL;
    } 

    (*buffer)++;

    node->left = node_left;
    node->right = node_right;

    return node;
}