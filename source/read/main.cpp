#include "tree.h"
#include "read.h"
#include "tree_logger.h"
#include "verificator.h"
#include "tokenizator.h"

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        fprintf(stderr, "ERROR: Not enough argument");
    }

    const char* in_filename = argv[1];
    const char* tree_filename = argv[2];

    Program* prog = Tokenize(in_filename);
    if(!prog) 
    {
        fprintf(stderr, "Error tokenizing file %s\n", in_filename);

        return 1;
    }

    Tree* tree = ReadProgramm(prog);

    free(prog->tokens);
    free(prog);

    if(!tree || !tree->root) 
    {
        fprintf(stderr, "Error processing file %s\n", in_filename);

        return 1;
    }

    SetParents(tree);

    tree = Verify(tree);

    TreeDotDump(tree);

    SaveTree(tree, tree_filename);

    TreeDestroy(tree);

    printf("Descent complete\n");

    return 0;
}