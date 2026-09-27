#include "TS2/cMaterialDefinition.h"
#include "common.h"

namespace nRZSceneGraph
{
    void cMaterialDefinition::Release()
    {
        using Func = void(__thiscall *)(cMaterialDefinition *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x8);
        vFunc(this);
    }

    void cMaterialDefinition::SetMaterialName(const char *matName)
    {
        using Func = void(__thiscall *)(cMaterialDefinition *, const char *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x28);
        vFunc(this, matName);
    }

    void cMaterialDefinition::SetDefinition(const char *matDef)
    {
        using Func = void(__thiscall *)(cMaterialDefinition *, const char *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x30);
        vFunc(this, matDef);
    }

    void cMaterialDefinition::SetParameter(const char *param, const char *value)
    {
        using Func = void(__thiscall *)(cMaterialDefinition *, const char *, const char *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x34);
        vFunc(this, param, value);
    }
}