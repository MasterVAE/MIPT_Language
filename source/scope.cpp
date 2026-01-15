#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "scope.h"

enum SearchState
{
    None,
    Use,
    Def
};

static void AddVarToNametable(TreeNode* node, Nametable* nametable);
static void SetNodeNametables(TreeNode* node, 
                              Nametable* parent_nametable, 
                              Nametable* base_nametable);

static Nametable* UpdateNametable(TreeNode* node, Nametable* old_nametable);
static SearchState SearchStateNode(TreeNode* node, const char* variable);

Nametable* GLOBAL_NAMETABLE = NULL;
size_t BLOCK_COUNT = 0; 

Nametable* CreateNametable()
{
    Nametable* nametable = (Nametable*)calloc(1, sizeof(Nametable));
    if(!nametable) return NULL;

    nametable->variable_count = 0;
    nametable->variables = (VariableData*)calloc(1, sizeof(VariableData));
    if(!nametable->variables) return NULL;

    return nametable;
}

void DestroyNametable(Nametable* nametable)
{
    if(!nametable) return;

    for(size_t i = 0; i < nametable->variable_count; i++)
    {
        free(nametable->variables[i].name);
        free(nametable->variables[i].scope_name);
    }

    free(nametable->variables);

    free(nametable);
}

Nametable* CopyNametable(Nametable* nametable)
{
    assert(nametable);
    
    Nametable* copy = CreateNametable();
    if(!copy) return NULL;

    copy->variable_count = nametable->variable_count;

    copy->variables = (VariableData*)realloc(copy->variables,
                                        (copy->variable_count == 0 ? 1 : copy->variable_count) *
                                        sizeof(VariableData));
    
    if(!copy->variables) return NULL;

    for(size_t i = 0; i < copy->variable_count; i++)
    {
        copy->variables[i].name = strdup(nametable->variables[i].name);
        copy->variables[i].scope_name = strdup(nametable->variables[i].scope_name);
    }

    return copy;
}

Nametable* CreateBasicNametable(Tree* tree)
{
    assert(tree);
    Nametable* nametable = CreateNametable();

    AddVarToNametable(tree->root, nametable);

    return nametable;
}

static void AddVarToNametable(TreeNode* node, Nametable* nametable)
{
    assert(nametable);

    if(!node) return;

    if(CheckOperation(node, OP_VARIABLE))
    {
        if(node->left)
        {
            char* name = node->left->value.identificator;

            if(FindVarNameInNametable(nametable, name) != -1) return;

            nametable->variable_count++;

            nametable->variables = (VariableData*)realloc(nametable->variables, 
                                            sizeof(VariableData) * nametable->variable_count);

            nametable->variables[nametable->variable_count - 1].name = strdup(name);
            nametable->variables[nametable->variable_count - 1].scope_name = strdup(name);


            GLOBAL_NAMETABLE->variable_count++;

            GLOBAL_NAMETABLE->variables = 
                (VariableData*)realloc(GLOBAL_NAMETABLE->variables, 
                                       sizeof(VariableData) * GLOBAL_NAMETABLE->variable_count);

            GLOBAL_NAMETABLE->variables[GLOBAL_NAMETABLE->variable_count - 1].name = 
                strdup(name);
            GLOBAL_NAMETABLE->variables[GLOBAL_NAMETABLE->variable_count - 1].scope_name = 
                strdup(name);
        }

        return;
    }


    AddVarToNametable(node->left, nametable);
    AddVarToNametable(node->right, nametable);
}

void SetNametables(Tree* tree)
{
    assert(tree);

    GLOBAL_NAMETABLE = CreateNametable();
    if(!GLOBAL_NAMETABLE) return;

    Nametable* root_nametable = CreateBasicNametable(tree);
    if(!root_nametable) return;

    SetNodeNametables(tree->root, root_nametable, root_nametable);

    DestroyNametable(root_nametable);
}

static void SetNodeNametables(TreeNode* node, 
                              Nametable* parent_nametable, 
                              Nametable* base_nametable)
{
    assert(parent_nametable);
    
    if(!node) return;

    if(CheckOperation(node, OP_VARIABLE))
    {
        node->nametable = CopyNametable(parent_nametable);
        return;
    }

    if(CheckOperation(node, OP_FUNCTION) 
    && CheckOperation(node->right, OP_FUNCTION)
    && node->left->type == NODE_IDENTIFICATOR
    && node->right->left->type != NODE_IDENTIFICATOR)
    {
        SetNodeNametables(node->left, base_nametable, base_nametable);
        SetNodeNametables(node->right, base_nametable, base_nametable);
    }
    else if(CheckOperation(node, OP_IF) || CheckOperation(node, OP_WHILE))
    {
        SetNodeNametables(node->left, parent_nametable, base_nametable);

        BLOCK_COUNT++;
        Nametable* updated_nametable = UpdateNametable(node->right, parent_nametable);

        SetNodeNametables(node->right, updated_nametable, base_nametable);

        DestroyNametable(updated_nametable);
    }
    else
    {
        SetNodeNametables(node->left, parent_nametable, base_nametable);
        SetNodeNametables(node->right, parent_nametable, base_nametable);
    }
}

int FindVarNameInNametable(Nametable* nametable, const char* variable)
{
    assert(nametable);
    assert(variable);

    for(int i = 0; i < (int)nametable->variable_count; i++)
    {
        if(!strcmp(nametable->variables[i].name, variable)) return i;
    }

    return -1;
}

int FindVarScopeInNametable(Nametable* nametable, const char* variable)
{
    assert(nametable);
    assert(variable);

    for(int i = 0; i < (int)nametable->variable_count; i++)
    {
        if(!strcmp(nametable->variables[i].scope_name, variable)) return i;
    }

    return -1;
}

int SearchFromLocalToGlobalNametable(Nametable* local_nametable, const char* variable)
{
    assert(local_nametable);
    assert(GLOBAL_NAMETABLE);
    assert(variable);

    int i = FindVarNameInNametable(local_nametable, variable);
    if(i == -1) return -1;

    return FindVarScopeInNametable(GLOBAL_NAMETABLE, local_nametable->variables[i].scope_name);
}

size_t GlobalNametableVariableCount()
{
    assert(GLOBAL_NAMETABLE);

    return GLOBAL_NAMETABLE->variable_count;
}

void ClearNametables(TreeNode* node)
{
    if(!node) return;

    DestroyNametable(node->nametable);

    ClearNametables(node->left);
    ClearNametables(node->right);
}

static Nametable* UpdateNametable(TreeNode* node, Nametable* old_nametable)
{
    assert(node);
    assert(old_nametable);

    Nametable* nametable = CopyNametable(old_nametable);
    if(!nametable) return NULL;

    for(size_t i = 0; i < nametable->variable_count; i++)
    {
        SearchState state = SearchStateNode(node, nametable->variables[i].name);
        if(state != Def) continue;

        char buffer[BUFFER_SIZE] = "";
        strcpy(buffer, nametable->variables[i].name);
        size_t len = strlen(nametable->variables[i].name);
        buffer[len] = '_';
        len++;
        sprintf(buffer + len, "%lu", BLOCK_COUNT);

        free(nametable->variables[i].scope_name);

        nametable->variables[i].scope_name = strdup(buffer);

        GLOBAL_NAMETABLE->variable_count++;

        GLOBAL_NAMETABLE->variables = 
            (VariableData*)realloc(GLOBAL_NAMETABLE->variables, 
                                   sizeof(VariableData) * GLOBAL_NAMETABLE->variable_count);

        GLOBAL_NAMETABLE->variables[GLOBAL_NAMETABLE->variable_count - 1].name = 
            strdup(nametable->variables[i].name);
        GLOBAL_NAMETABLE->variables[GLOBAL_NAMETABLE->variable_count - 1].scope_name = 
            strdup(nametable->variables[i].scope_name);
    }

    return nametable;
}

static SearchState SearchStateNode(TreeNode* node, const char* variable)
{
    assert(variable);

    if(!node) return None;

    if(CheckOperation(node, OP_VARIABLE))
    {
        if(!node->left) return None;
        const char* name = node->left->value.identificator;

        if(strcmp(name, variable)) return None;

        return Use;
    }
    else if(CheckOperation(node, OP_ASSIGN))
    {
        if(!node->left || !node->left->left) return None;
        const char* name = node->left->left->value.identificator;

        if(strcmp(name, variable)) return None;

        return Def;
    }

    if(CheckOperation(node, OP_FUNCTION) 
    && CheckOperation(node->right, OP_FUNCTION)
    && node->left->type == NODE_IDENTIFICATOR
    && node->right->left->type != NODE_IDENTIFICATOR)
    {
        return None;
    }

    if(CheckOperation(node, OP_IF) || CheckOperation(node, OP_WHILE))
    {
        return None;
    }

    SearchState state_1 = SearchStateNode(node->left, variable);
    SearchState state_2 = SearchStateNode(node->right, variable);

    return (state_1 == None) ? state_2 : state_1;
}

