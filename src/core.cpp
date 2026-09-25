#include "core.h"
#include "hooking.h"
#include "maps.h"
#include "terrain.h"
#include "lotskirt.h"
#include "shaders.h"

namespace Core
{
    void InjectPatches()
    {
        Maps::RemoveMapSizeLimit();
        Hooking::MakeJMP((BYTE *)0x102C94C, (DWORD)Terrain::GetIsBeachLot, 5);
        Hooking::MakeJMP((BYTE *)0xEFC9CB, (DWORD)Terrain::GetLotTemplate, 5);
        Hooking::MakeJMP((BYTE *)0xAE23BA, (DWORD)Terrain::AddTerrainPaintParams, 5);
        Hooking::MakeJMP((BYTE *)0xAE2EC8, (DWORD)Terrain::AddTerrainCanvasParams, 5);
        Hooking::MakeJMP((BYTE *)0xA83747, (DWORD)LotSkirt::HandleRoadParams, 7);
        Hooking::MakeJMP((BYTE *)0xA84329, (DWORD)LotSkirt::AddLotSkirtRoadParams, 6);
        Hooking::MakeJMP((BYTE *)0xB24C7D, (DWORD)Shaders::AddWeatherShaderMaterials, 5);
    }
}