#pragma once
#include "TS2/cGZCOM.h"
#include "TS2/cWorldDB.h"
#include "TS2/cTSGlobals.h"
#include "TS2/cTSGameStateController.h"
#include "TS2/cMaterialManager.h"

using GetGZCOM = cGZCOM *(__cdecl *)();
extern GetGZCOM GZCOM;

namespace TS
{
    using GetWorldDB = nTSWorld::cWorldDB *(__cdecl *)();
    extern GetWorldDB WorldDB;

    using GetGlobals = cTSGlobals *(__cdecl *)();
    extern GetGlobals Globals;

    using GetGameStateController = cTSGameStateController *(__cdecl *)();
    extern GetGameStateController GameStateController;
}

namespace nRZSceneGraph
{
    using GetMaterialManager = cMaterialManager *(__cdecl *)();
    extern GetMaterialManager MaterialManager;
}