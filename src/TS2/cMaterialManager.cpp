#include "TS2/cMaterialManager.h"
#include "common.h"

namespace nRZSceneGraph
{
    void cMaterialManager::RegisterMaterialDefinition(cMaterialDefinition *matDef, int flag)
    {
        using Func = void(__thiscall *)(cMaterialManager *, cMaterialDefinition *, int);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x5C);
        vFunc(this, matDef, flag);
    }
}