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