#pragma once
#include "TS2/cGZCOM.h"
#include "TS2/cMaterialManager.h"

using GetGZCOM = cGZCOM *(__cdecl *)();
extern GetGZCOM GZCOM;

namespace TS
{
    extern const DWORD Globals;
}

namespace nRZSceneGraph
{
    using GetMaterialManager = cMaterialManager *(__cdecl *)();
    extern GetMaterialManager MaterialManager;
}