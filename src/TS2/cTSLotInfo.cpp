#include "TS2/cTSLotInfo.h"
#include "common.h"

cTSBaseLotInfo *cTSLotInfo::GetBaseLotInfo()
{
    using Func = cTSBaseLotInfo *(__thiscall *)(cTSLotInfo *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x10);
    return vFunc(this);
}

void cTSLotInfo::GetLocation(ULONG32 *x, ULONG32 *y)
{
    using Func = void(__thiscall *)(cTSLotInfo *, ULONG32 *, ULONG32 *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x28);
    vFunc(this, x, y);
}

float cTSLotInfo::NHoodToLotHeightOffset()
{
    using Func = float(__thiscall *)(cTSLotInfo *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x40);
    return vFunc(this);
}

BYTE cTSLotInfo::RotationRelativeToCreation()
{
    using Func = BYTE(__thiscall *)(cTSLotInfo *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x5C);
    return vFunc(this);
}