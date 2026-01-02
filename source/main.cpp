#include "tree.h"
#include "read.h"
#include "tree_logger.h"
#include "verificator.h"
#include "compilator.h"
#include "tokenizator.h"

static const char* const in_filename = "files/prog.upl";
static const char* const out_filename = "files/code.asm";

// FIXME сначала посчитать переменные

int main()
{
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

    return 0;
}