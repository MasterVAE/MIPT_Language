#include "tree.h"
#include "read.h"
#include "tree_logger.h"
#include "verificator.h"
#include "tokenizator.h"

static const char* const in_filename = "files/prog.upl";
static const char* const out_filename = "files/code.asm";
static const char* const tree_filename = "files/tree.tr";

int main()
{
    // TODO arc agv

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