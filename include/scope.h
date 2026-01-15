#ifndef SCOPE_H
#define SCOPE_H

#include "tree.h"

Nametable* CreateNametable();
void DestroyNametable(Nametable* nametable);

Nametable* CopyNametable(Nametable* nametable);

Nametable* CreateBasicNametable(Tree* tree);

void SetNametables(Tree* tree);

int FindVarNameInNametable(Nametable* nametable, const char* variable);
int FindVarScopeInNametable(Nametable* nametable, const char* variable);
int SearchFromLocalToGlobalNametable(Nametable* local_nametable, const char* variable);

size_t GlobalNametableVariableCount();

void ClearNametables(TreeNode* node);

#endif // SCOPE_H