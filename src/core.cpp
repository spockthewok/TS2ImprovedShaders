#include "core.h"

namespace Core
{
    void InjectPatches()
    {
        Maps::RemoveMapSizeLimit();
        Hooking::MakeJMP((BYTE *)0x102C94C, (DWORD)Shaders::GetIsBeachLot, 5);
        Hooking::MakeJMP((BYTE *)0xEFC9CB, (DWORD)Shaders::GetLotTemplate, 5);
        Hooking::MakeJMP((BYTE *)0xAE23BA, (DWORD)Shaders::AddTerrainPaintParams, 5);
        Hooking::MakeJMP((BYTE *)0xAE2EC8, (DWORD)Shaders::AddTerrainCanvasParams, 5);
        Hooking::MakeJMP((BYTE *)0xA8376D, (DWORD)Shaders::AddLotSkirtParams, 5);
    }
}