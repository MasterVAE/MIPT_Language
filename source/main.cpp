#include <stdio.h>

#include "tree.h"
#include "read.h"
#include "tree_logger.h"
#include "verificator.h"
#include "compilator.h"

static const char* const filename = "/home/hobie/Documents/Projects/MIPT_Stack/files/code.asm";

int main()
{
    Tree* tree = ReadProgramm("files/prog.upl");
    SetParents(tree);

    tree = Verify(tree);

    TreeDotDump(tree);

    FILE* file = fopen(filename, "w+");
    if(!file)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        TreeDestroy(tree);
    }

    CompileTree(tree, file);

    fclose(file);

    TreeDestroy(tree);
}