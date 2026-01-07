#include "tree.h"
#include "tree_logger.h"
#include "compilator.h"

static const char* const out_filename = "files/code.asm";
static const char* const tree_filename = "files/tree.tr";

int main()
{
    // TODO arc agv

    Tree* tree = LoadTree(tree_filename);
    if(!tree)
    {
        fprintf(stderr, "Error loading tree from %s\n", tree_filename);

        return 1;
    }

    FILE* file = fopen(out_filename, "w+");
    if(!file)
    {
        fprintf(stderr, "Error opening file %s\n", out_filename);
        TreeDestroy(tree);

        return 1;
    }

    CompileTree(tree, file);

    fclose(file);

    TreeDestroy(tree);

    printf("Compilation complete\n");

    return 0;
}