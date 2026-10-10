#include "TS2/base.h"

// Base addresses of various TS2 methods

GetGZCOM GZCOM = reinterpret_cast<GetGZCOM>(0x40F318);

namespace TS
{
    GetWorldDB WorldDB = reinterpret_cast<GetWorldDB>(0x4AE0DC);
    GetGlobals Globals = reinterpret_cast<GetGlobals>(0x799A0D);
    GetGameStateController GameStateController = reinterpret_cast<GetGameStateController>(0x799B65);
}

namespace nRZSceneGraph
{
    GetMaterialManager MaterialManager = reinterpret_cast<GetMaterialManager>(0xE745A0);
}