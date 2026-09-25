#include "terrain.h"
#include "hooking.h"
#include <string>

namespace
{
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit = 0xAE2ED0;
    const DWORD LoadLot_Exit = 0xEFC9D0;
    const DWORD Load_Exit = 0x102C951;

    const size_t paramMax = 11;

    // Lot width/height
    const char lotXScaleParam[] = "lotXScale";
    char lotXScale[paramMax];
    const char lotYScaleParam[] = "lotYScale";
    char lotYScale[paramMax];

    const char isBeachLotParam[] = "isBeachLot";
    char isBeachLot[6];
}

namespace Terrain
{
    // Condensed version of TSGetLotXScale and TSGetLotYScale from RPCLib
    // https://github.com/LazyDuchess/RPCLib/blob/master/RPCLib/common.cpp
    static char *GetLotScale(BYTE addrOffset, char *lotAxis)
    {
        DWORD addr = 0x1478F10;
        const size_t addrSize = sizeof(addr);

        if (Hooking::MemoryReadable((DWORD *)addr, addrSize))
        {
            memcpy_s(&addr, addrSize, (DWORD *)addr, addrSize);
            addr += 0x80;
            if (Hooking::MemoryReadable((DWORD *)addr, addrSize))
            {
                memcpy_s(&addr, addrSize, (DWORD *)addr, addrSize);
                addr += addrOffset;
                if (Hooking::MemoryReadable((DWORD *)addr, addrSize))
                {
                    memcpy_s(&addr, addrSize, (DWORD *)addr, addrSize);
                    // sizeof(lotAxis) would return pointer size, not array size
                    strcpy_s(lotAxis, paramMax, std::to_string(addr).c_str());
                    return lotAxis;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        // RPCLib returns lotXScale/lotYScale regardless
        strcpy_s(lotAxis, paramMax, "0");
        return lotAxis;
    }

    static void SetIsBeachLotParam(bool isBeach)
    {
        if (isBeach)
            strcpy_s(isBeachLot, sizeof(isBeachLot), "true");
        else
            strcpy_s(isBeachLot, sizeof(isBeachLot), "false");
    }

    static void GetIsBeachLotFromStr(const char *lotTemplate)
    {
        bool isBeach;

        if (!lotTemplate)
            isBeach = false;
        else
            // Beaches will either be "BeachCommunityLotTemplate" or "BeachLotTemplate"
            isBeach = (_strnicmp(lotTemplate, "Beach", 5) == 0);

        SetIsBeachLotParam(isBeach);
    }

    // cWorldDB::Load
    // Used for premade lots/lots from lot bin
    void __declspec(naked) GetIsBeachLot()
    {
        __asm {
            call [eax+0x60]
            push eax
            call SetIsBeachLotParam
            pop eax
            test al,al
            jmp Load_Exit
        }
    }

    // cTSLoadLotController::LoadLot
    // Used for lots created from empty lot templates
    void __declspec(naked) GetLotTemplate()
    {
        __asm {
            mov byte ptr [ebp-0x4],0xA
            pushad
            push edi
            call GetIsBeachLotFromStr
            add esp,0x4
            popad
            push edi
            jmp LoadLot_Exit
        }
    }

    // cTerrain::RegisterPaintMaterialDefinition
    // Adds extra parameters to lot terrain paint material
    void __declspec(naked) AddTerrainPaintParams()
    {
        __asm {
            push 0x123EAA4 // "alphaMapScaleV"
            call [eax+0x34]
            push offset lotXScale
            push 0x64
            call GetLotScale
            add esp,0x8
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax // lotXScale
            push offset lotXScaleParam
            call [edx+0x34]
            push offset lotYScale
            push 0x68
            call GetLotScale
            add esp,0x8
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax // lotYScale
            push offset lotYScaleParam
            call [edx+0x34]
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset isBeachLot
            push offset isBeachLotParam
            call [edx+0x34]
            jmp RegisterPaintMaterialDefinition_Exit
        }
    }

    // cTerrain::RegisterCanvasMaterialDefinition
    // Adds extra parameters to lot terrain canvas material
    // Runs shortly after paint hook, so don't need to call lot scale getter again
    void __declspec(naked) AddTerrainCanvasParams()
    {
        __asm {
            push 0x123EB5C // "texture"
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotXScale
            push offset lotXScaleParam
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotYScale
            push offset lotYScaleParam
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset isBeachLot
            push offset isBeachLotParam
            call [edx+0x34]
            jmp RegisterCanvasMaterialDefinition_Exit
        }
    }
}