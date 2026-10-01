#pragma once
#include "TS2/cGZCOM.h"
#include "TS2/cMaterialManager.h"
#include "TS2/cTSSGSystem.h"

using GetGZCOM = cGZCOM *(__cdecl *)();
extern GetGZCOM GZCOM;

namespace nTSWorld
{
    using GetTSSGSystem = nTSSG::cTSSGSystem *(__cdecl *)();
    extern GetTSSGSystem TSSGSystem;
}

namespace TS
{
    extern const DWORD Globals;
}

namespace nRZSceneGraph
{
    using GetMaterialManager = cMaterialManager *(__cdecl *)();
    extern GetMaterialManager MaterialManager;
}