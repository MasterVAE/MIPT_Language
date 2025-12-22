#include <stdio.h>

#include "tree.h"
#include "read.h"
#include "tree_logger.h"
#include "verificator.h"

int main()
{
    Tree* tree = ReadProgramm("files/prog.upl");
    SetParents(tree);

    tree = Verify(tree);

    TreeDotDump(tree);


    TreeDestroy(tree);
}