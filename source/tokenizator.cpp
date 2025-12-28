#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "tokenizator.h"
#include "op_data.h"
#include "tree.h"

static size_t FileLength(FILE* file);
static Program* Read(Program* prog, char* buffer);
static char* SkipSpaces(char* buffer);
static Program* AddToken(Program* prog, NodeType type, NodeValue value);

#define SKIP buffer = SkipSpaces(buffer);

Program* Tokenize(const char* filename)
{
    assert(filename);

    FILE* file = fopen(filename, "r");
    if(!file) return NULL;

    size_t lenght = FileLength(file);
    char* buffer = (char*)calloc(lenght + 1, sizeof(char));
    if(!buffer) return NULL;

    Program* prog = (Program*)calloc(1, sizeof(Program));
    if(!prog) return NULL;

    prog->token_count = 0;
    prog->tokens = (Token*)calloc(1, sizeof(Token));

    if(!prog->tokens)
    {
        free(prog);
        return NULL;
    }

    fread(buffer, lenght, 1, file);
    buffer[lenght] = '\0';

    char* buffer_copy = buffer;

    Read(prog, buffer_copy);

    free(buffer);
    fclose(file);
    return prog;
}

static Program* Read(Program* prog, char* buffer)
{
    assert(prog);
    assert(buffer);

    while(*buffer != '\0')
    {
        SKIP;

        int value = 0;
        int count = 0;
        sscanf(buffer, "%d%n", &value, &count);

        buffer += count;

        if(count > 0)
        {
            prog = AddToken(prog, NODE_CONSTANT, NodeValue {.constant = value});
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
                prog = AddToken(prog, NODE_OPERATION, NodeValue {.operation = data.op});
                if(!prog) return NULL;
            
                SKIP;
                found = true;
                buffer += strlen(data.op_code);
                break;
            }
        }

        if(found) continue;

        size_t len = 0;
        char* start = buffer;
        while(!IsSystem(*buffer))
        {
            len += 1;
            buffer++;
        }

        if(len > 0)
        {
            prog = AddToken(prog, NODE_IDENTIFICATOR, NodeValue {.identificator = strndup(start, len)});
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

static Program* AddToken(Program* prog, NodeType type, NodeValue value)
{
    prog->token_count++;

    Token* new_tokens = (Token*)realloc(prog->tokens, prog->token_count * sizeof(Token));
    if(!new_tokens)
    {
        free(prog->tokens);
        free(prog);
        return NULL;
    }

    prog->tokens = new_tokens;

    Token* token = &prog->tokens[prog->token_count - 1];
    token->type = type;
    token->value = value;

    return prog;
}

static char* SkipSpaces(char* buffer)
{
    assert(buffer);

    while(isspace(*buffer)) buffer++;

    return buffer;
}

static size_t FileLength(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t filesize = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    return filesize;
}