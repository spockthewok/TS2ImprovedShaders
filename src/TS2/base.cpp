#include "TS2/base.h"

// Base addresses of various TS2 methods

GetGZCOM GZCOM = reinterpret_cast<GetGZCOM>(0x40F318);

namespace TS
{
    const DWORD Globals = 0x799A0D;
}

namespace nRZSceneGraph
{
    GetMaterialManager MaterialManager = reinterpret_cast<GetMaterialManager>(0xE745A0);
}