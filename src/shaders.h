#pragma once
#include "headers.h"
#include "hooking.h"

namespace Shaders
{
    extern "C" void GetIsBeachLot();
    extern "C" void GetLotTemplate();
    extern "C" void TerrainPaintHook();
    extern "C" void TerrainCanvasHook();
}