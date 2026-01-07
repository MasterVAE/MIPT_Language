#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "tokenizator.h"
#include "op_data.h"
#include "library.h"
#include "tree.h"

static Program* Read(Program* prog, const char* buffer);
static size_t SkipSpaces(const char** buffer);
static Program* AddToken(Program* prog, size_t line, NodeType type, NodeValue value);

#define SKIP line = SkipSpaces(&buffer);

Program* Tokenize(const char* filename)
{
    assert(filename);

    char* buffer = ReadFile(filename);
    if(!buffer) return NULL;

    Program* prog = (Program*)calloc(1, sizeof(Program));
    if(!prog) return NULL;

    prog->token_count = 0;
    prog->current_token = 0;
    prog->tokens = (Token*)calloc(1, sizeof(Token));

    if(!prog->tokens) return NULL;

    char* buffer_copy = buffer;

    Read(prog, buffer_copy);

    free(buffer);
    return prog;
}

static Program* Read(Program* prog, const char* buffer)
{
    assert(prog);
    assert(buffer);

    size_t line = 0;

    while(*buffer != '\0')
    {
        SKIP;

        int value = 0;
        int count = 0;
        sscanf(buffer, "%d%n", &value, &count);

        buffer += count;

        if(count > 0)
        {
            prog = AddToken(prog, line, NODE_CONSTANT, NodeValue {.constant = value});
            if(!prog) return NULL;
            
            SKIP;
            continue;
        }


        bool found = false;

        for(size_t i = 0; i < OP_COUNT; i++)
        {
            Oper data = OP_DATA[i];
            if(!data.op_code) continue;

            if(!strncmp(buffer, data.op_code, strlen(data.op_code)))
            {
                prog = AddToken(prog, line, NODE_OPERATION, NodeValue {.operation = data.op});
                if(!prog) return NULL;
            
                SKIP;
                found = true;
                buffer += strlen(data.op_code);
                break;
            }
        }

        if(found) continue;

        size_t len = 0;
        const char* start = buffer;
        while(!IsSystem(*buffer))
        {
            len += 1;
            buffer++;
        }

        if(len > 0)
        {
            prog = AddToken(prog, line, NODE_IDENTIFICATOR, 
                                        NodeValue {.identificator = strndup(start, len)});
            if(!prog) return NULL;
        }
        else
        {
            fprintf(stderr, "Unknown character %s\n", buffer);
        }

        SKIP;
    }

    return prog;
}

static Program* AddToken(Program* prog, size_t line, NodeType type, NodeValue value)
{
    assert(prog);

    prog->token_count++;

    Token* new_tokens = (Token*)realloc(prog->tokens, prog->token_count * sizeof(Token));
    if(!new_tokens) return NULL;

    prog->tokens = new_tokens;

    Token* token = &prog->tokens[prog->token_count - 1];
    token->line = line;
    token->type = type;
    token->value = value;

    return prog;
}

static size_t SkipSpaces(const char** buffer)
{
    assert(buffer);

    static size_t line = 0;

    while(isspace(**buffer))
    {
        if(**buffer == '\n')
        {
            line++;
        }

        (*buffer)++;
    }

    return line;
}
