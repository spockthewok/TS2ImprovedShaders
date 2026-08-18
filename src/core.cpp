#include "core.h"

namespace Core
{
    void InjectPatches()
    {
        Maps::RemoveMapSizeLimit();
        Hooking::MakeJMP((BYTE *)0x102C94C, (DWORD)Shaders::GetIsBeachLot, 5);
        Hooking::MakeJMP((BYTE *)0xEFC9CB, (DWORD)Shaders::GetLotTemplate, 5);
        Hooking::MakeJMP((BYTE *)0xAE23BA, (DWORD)Shaders::TerrainPaintHook, 5);
        Hooking::MakeJMP((BYTE *)0xAE2EC8, (DWORD)Shaders::TerrainCanvasHook, 5);
    }
}