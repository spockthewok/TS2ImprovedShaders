#include "TS2/cTSBaseLotInfo.h"
#include "common.h"

void cTSBaseLotInfo::GetSizeAtCreation(ULONG32 *width, ULONG32 *height)
{
    using Func = void(__thiscall *)(cTSBaseLotInfo *, ULONG32 *, ULONG32 *);
    auto vFunc = Common::GetVTableFn<Func>(this, 0x14);
    vFunc(this, width, height);
}