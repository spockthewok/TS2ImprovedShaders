#include "core.h"
#include "hooking.h"
#include "maps.h"
#include "shaders.h"

namespace Core
{
    void InjectPatches()
    {
        Maps::RemoveMapSizeLimit();
        Hooking::MakeJMP((BYTE *)0x102C94C, (DWORD)Shaders::GetIsBeachLot, 5);
        Hooking::MakeJMP((BYTE *)0xEFC9CB, (DWORD)Shaders::GetLotTemplate, 5);
        Hooking::MakeJMP((BYTE *)0xAE23BA, (DWORD)Shaders::AddTerrainPaintParams, 5);
        Hooking::MakeJMP((BYTE *)0xAE2EC8, (DWORD)Shaders::AddTerrainCanvasParams, 5);
        Hooking::MakeJMP((BYTE *)0xA83747, (DWORD)Shaders::AddLotSkirtParams, 7);
        Hooking::MakeJMP((BYTE *)0xA84329, (DWORD)Shaders::AddLotSkirtRoadParams, 6);
        Hooking::MakeJMP((BYTE *)0xB24C7D, (DWORD)Shaders::AddWeatherShaderMaterials, 5);
    }
}