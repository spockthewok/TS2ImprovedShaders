#include "TS2/cLotSkirt.h"

namespace nTSSG
{
    void cLotSkirt::ComputeNHoodToLotTransformationParameters()
    {
        using Func = void(__thiscall *)(cLotSkirt *);
        auto func = reinterpret_cast<Func>(0xA7D780);
        func(this);
    }
}