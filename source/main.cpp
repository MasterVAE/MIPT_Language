#include <stdio.h>

#include "tree.h"
#include "read.h"
#include "tree_logger.h"

int main()
{
    Tree* tree = ReadProgramm("files/prog.upl");
 
    TreeDotDump(tree);

    TreeDestroy(tree);
}