#include "shaders.h"

namespace
{
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit = 0xAE2ED0;
    const DWORD LoadLot_Exit = 0xEFC9D0;
    const DWORD Load_Exit = 0x102C951;

    const char lotXSizeParam[] = "lotXScale";
    const char lotYSizeParam[] = "lotYScale";
    // X/Y size should only be 2-3 digits, but better to be safe than sorry
    // Will allow 32-bit values to be stored in array
    const size_t lotSizeMax = 11;
    char lotXSize[lotSizeMax];
    char lotYSize[lotSizeMax];

    const char isBeachParam[] = "isBeachLot";
    char isBeachLot[6];
}

namespace Shaders
{
    // Condensed version of TSGetLotXScale and TSGetLotYScale from RPCLib
    // https://github.com/LazyDuchess/RPCLib/blob/master/RPCLib/common.cpp
    static char *GetLotScale(DWORD addrOffset, char *lotAxis)
    {
        DWORD addr = 0x1478F10;
        if (Hooking::MemoryReadable((DWORD *)addr, 4))
        {
            memcpy_s(&addr, 4, (DWORD *)addr, 4);
            addr += 0x80;
            if (Hooking::MemoryReadable((DWORD *)addr, 4))
            {
                memcpy_s(&addr, 4, (DWORD *)addr, 4);
                addr += addrOffset;
                if (Hooking::MemoryReadable((DWORD *)addr, 4))
                {
                    memcpy_s(&addr, 4, (DWORD *)addr, 4);
                    // sizeof(lotAxis) would return pointer size, not array size
                    strcpy_s(lotAxis, lotSizeMax, std::to_string(addr).c_str());
                    return lotAxis;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        // RPCLib returns lotXSize/lotYSize regardless
        strcpy_s(lotAxis, lotSizeMax, "0");
        return lotAxis;
    }

    // Parameter expects a string rather than a boolean
    static void SetBeachParamValue(bool isBeach)
    {
        if (isBeach)
            strcpy_s(isBeachLot, sizeof(isBeachLot), "true");
        else
            strcpy_s(isBeachLot, sizeof(isBeachLot), "false");
    }

    static void GetIsBeachFromStr(const char *lotTemplate)
    {
        bool isBeach;

        if (!lotTemplate)
            isBeach = false;
        else
            // Beaches will either be "BeachCommunityLotTemplate" or "BeachLotTemplate"
            isBeach = (_strnicmp(lotTemplate, "Beach", 5) == 0);

        SetBeachParamValue(isBeach);
    }

    // cWorldDB::Load
    // Used for premade lots/lots from lot bin
    void __declspec(naked) GetIsBeachLot()
    {
        __asm {
            call [eax+0x60]
            push eax
            call SetBeachParamValue
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
            call GetIsBeachFromStr
            add esp,0x4
            popad
            push edi
            jmp LoadLot_Exit
        }
    }

    // cTerrain::RegisterPaintMaterialDefinition
    // Adds extra parameters to lot terrain paint shader
    void __declspec(naked) AddTerrainPaintParams()
    {
        __asm {
            push 0x123EAA4 // "alphaMapScaleV"
            call [eax+0x34]
            push offset lotXSize
            push 0x64
            call GetLotScale
            add esp,0x8
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax // lotXSize
            push offset lotXSizeParam
            call [edx+0x34]
            push offset lotYSize
            push 0x68
            call GetLotScale
            add esp,0x8
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax // lotYSize
            push offset lotYSizeParam
            call [edx+0x34]
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset isBeachLot
            push offset isBeachParam
            call [edx+0x34]
            jmp RegisterPaintMaterialDefinition_Exit
        }
    }

    // cTerrain::RegisterCanvasMaterialDefinition
    // Adds extra parameters to lot terrain canvas shader
    // Runs shortly after the paint hook, so don't need to call lot size getter again
    void __declspec(naked) AddTerrainCanvasParams()
    {
        __asm {
            push 0x123EB5C // "texture"
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotXSize
            push offset lotXSizeParam
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotYSize
            push offset lotYSizeParam
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset isBeachLot
            push offset isBeachParam
            call [edx+0x34]
            jmp RegisterCanvasMaterialDefinition_Exit
        }
    }
}