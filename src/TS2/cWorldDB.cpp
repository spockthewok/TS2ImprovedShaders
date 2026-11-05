#include "TS2/cWorldDB.h"
#include "common.h"

namespace nTSWorld
{
    int cWorldDB::NumTilesX()
    {
        using Func = int(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x154);
        return vFunc(this);
    }

    int cWorldDB::NumTilesY()
    {
        using Func = int(__thiscall *)(cWorldDB *);
        auto vFunc = Common::GetVTableFn<Func>(this, 0x158);
        return vFunc(this);
    }
}