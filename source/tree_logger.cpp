#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tree.h"
#include "tree_logger.h"
#include "op_data.h"

static const char* const DOT_FILENAME = "files/dot.dt";
static const char* const TEX_FILENAME = "files/tex.tx";

static void TreeDotDumpStart(FILE* file);
static void TreeDotDumpEnd(FILE* file);

static void DotPrintNode(TreeNode* node, FILE* file, int rank);
static void DotPrintValue(TreeNode* node, FILE* file);

void TreeDotDump(Tree* tree)
{
    static size_t dot_num = 0;

    assert(tree);

    FILE* file = fopen(DOT_FILENAME, "w+");
    if(!file) return;

    dot_num++;

    TreeDotDumpStart(file);
    DotPrintNode(tree->root, file, 0);
    TreeDotDumpEnd(file);

    fclose(file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tpng %s -o files/tree%lu.png", DOT_FILENAME, dot_num);
    system(command);
}

static void DotPrintNode(TreeNode* node, FILE* file, int rank)
{
    assert(node);
    assert(file);

    fprintf(file, "\"%p_node\" [label = ", node);
    DotPrintValue(node, file);
    fprintf(file, "]; \n");

    if (node->left)
    {
        fprintf(file, "\"%p_node\" -> \"%p_node\";\n", node, node->left);
        DotPrintNode(node->left, file, rank + 1);
    }
    if (node->right)
    {
        fprintf(file, "\"%p_node\" -> \"%p_node\";\n", node, node->right);
        DotPrintNode(node->right, file, rank + 1);
    }
}

static void TreeDotDumpStart(FILE* file)
{
    assert(file);

    fprintf(file, "digraph G {"
                  "\t node [shape= record, style=\"filled\"];\n"
                  "\t rankdir=TB;\n"
                );
}

static void TreeDotDumpEnd(FILE* file)
{
    assert(file);

    fprintf(file, "}\n");
}

static void DotPrintValue(TreeNode* node, FILE* file)
{
    assert(node);
    assert(file);

    if(node->type == NODE_OPERATION)
    {
        fprintf(file, "%s", GetOpName(node->value.operation));
    }
    else if(node->type == NODE_VARIABLE)
    {
        fprintf(file, "%c", node->value.variable);
    }
    else if(node->type == NODE_CONSTANT)
    {
        fprintf(file, "%d", node->value.constant);
    }
}
