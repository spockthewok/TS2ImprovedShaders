#include "shaders.h"

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit = 0xAE2ED0;
    const DWORD LoadLot_Exit = 0xEFC9D0;
    const DWORD Load_Exit = 0x102C951;

    const size_t lotSizeMax = 11;

    const char lotXScaleParam[] = "lotXScale";
    char lotXScale[lotSizeMax];
    const char lotYScaleParam[] = "lotYScale";
    char lotYScale[lotSizeMax];

    const char lotZPosParam[] = "lotZPos";
    char lotZPos[lotSizeMax];

    const char isBeachLotParam[] = "isBeachLot";
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
    static void SetIsBeachParam(bool isBeach)
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

        SetIsBeachParam(isBeach);
    }

    static void SetLotZPosParam(const float currZPos)
    {
        strcpy_s(lotZPos, sizeof(lotZPos), std::to_string(currZPos).c_str());
    }

    // cWorldDB::Load
    // Used for premade lots/lots from lot bin
    void __declspec(naked) GetIsBeachLot()
    {
        __asm {
            call [eax+0x60]
            push eax
            call SetIsBeachParam
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
    // Adds extra parameters to lot terrain canvas shader
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

    // cLotSkirt::RegisterMaterials
    // Adds extra parameters to lot skirt shader
    void __declspec(naked) AddLotSkirtParams()
    {
        __asm {
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            fld [edi+0xBC] // Sea level relative to lot z
            fchs // Negate to get lot z relative to sea level
            fstp [esp+0x18]
            push [esp+0x18]
            call SetLotZPosParam
            add esp,0x4
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotZPos
            push offset lotZPosParam
            call [edx+0x34]
            jmp RegisterMaterials_Exit
        }
    }
}