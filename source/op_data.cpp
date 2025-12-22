#include <stdlib.h>
#include <stdio.h>

#include "op_data.h"

const char* GetOpName(Operation operation)
{
    for(size_t op = 0; op < OP_COUNT; op++)
    {
        if(OP_DATA[op].op == operation)
        {
            return OP_DATA[op].op_name;
        }
    }

    return NULL;
}

bool IsSystem(char symbol)
{
    bool found = false;
    
    for(size_t i = 0; i < SYSTEM_SYMBOLS_COUNT; i++)
    {
        if(symbol == SYSTEM_SYMBOLS[i])
        {
            found = true;
            break;
        }
    }

    return found;
}