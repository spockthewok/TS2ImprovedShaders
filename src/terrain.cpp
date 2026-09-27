#include "terrain.h"
#include "hooking.h"
#include "TS2/base.h"
#include "common.h"

namespace
{
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit = 0xAE2ED0;
    const DWORD LoadLot_Exit = 0xEFC9D0;
    const DWORD Load_Exit = 0x102C951;

    // Lot width/height
    const char lotXScaleParam[] = "lotXScale";
    char lotXScale[Common::intMax];
    const char lotYScaleParam[] = "lotYScale";
    char lotYScale[Common::intMax];

    const char isBeachLotParam[] = "isBeachLot";
    char isBeachLot[Common::boolMax];
}

namespace Terrain
{
    // Condensed version of TSGetLotXScale and TSGetLotYScale from RPCLib
    // https://github.com/LazyDuchess/RPCLib/blob/master/RPCLib/common.cpp
    static char *GetLotScale(char *lotAxis, const DWORD addrOffset)
    {
        DWORD addr = 0x1478F10;
        constexpr size_t addrSize = sizeof(addr);

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
                    Common::SetParamInt(lotAxis, Common::intMax, addr);
                    return lotAxis;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        // RPCLib returns lotXScale/lotYScale regardless
        Common::SetParamInt(lotAxis, Common::intMax, 0);
        return lotAxis;
    }

    static void GetIsBeachLotFromStr(const char *lotTemplate)
    {
        bool isBeach;

        if (!lotTemplate)
            isBeach = false;
        else
            // Beaches will either be "BeachCommunityLotTemplate" or "BeachLotTemplate"
            isBeach = (_strnicmp(lotTemplate, "Beach", 5) == 0);

        Common::SetParamBool(isBeachLot, sizeof(isBeachLot), isBeach);
    }

    // cWorldDB::Load
    // Used for premade lots/lots from lot bin
    void __declspec(naked) GetIsBeachLot()
    {
        __asm {
            call [eax+0x60]
            push eax
            push [Common::boolMax]
            push offset isBeachLot
            call Common::SetParamBool
            add esp,0x8
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

    // This does very similar calculations to cLotSkirt::ComputeLotSkirtParameters
    // Terrain materials are set up before lot skirt, which is why we need this here
    static void __declspec(naked) GetLotZPos()
    {
        __asm {
            call TS::Globals
            mov [esp+0x48],eax
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x5C] // cTSGlobals::GameStateController
            test eax,eax
            jz LAB_Return
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x24] // cTSGameStateController::CurrentLotInfo
            test eax,eax
            jz LAB_Return
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x40] // cTSLotInfo::NHoodToLotHeightOffset
            mov eax,[esp+0x48]
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x2C] // cTSGlobals::NeighborhoodTerrain
            test eax,eax
            jz LAB_Return
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x40] // cTSNHoodTerrain::SeaLevel
            fsubp st(1),st(0) // lotZPos = lotHeightOffset - seaLevel
            fstp [esp+0x48]
            push [esp+0x48]
            push [Common::floatMax]
            push offset Common::lotZPos
            call Common::SetParamFloat
            add esp,0xC
        LAB_Return:
            ret
        }
    }

    // cTerrain::RegisterPaintMaterialDefinition
    // Adds extra parameters to lot terrain paint material
    void __declspec(naked) AddTerrainPaintParams()
    {
        __asm {
            push 0x123EAA4 // "alphaMapScaleV"
            call [eax+0x34]
            push 0x64
            push offset lotXScale
            call GetLotScale
            add esp,0x8
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax // lotXScale
            push offset lotXScaleParam
            call [edx+0x34]
            push 0x68
            push offset lotYScale
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
            call GetLotZPos
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset Common::lotZPos
            push offset Common::lotZPosParam
            call [edx+0x34]
            jmp RegisterPaintMaterialDefinition_Exit
        }
    }

    // cTerrain::RegisterCanvasMaterialDefinition
    // Adds extra parameters to lot terrain canvas material
    // Runs shortly after paint hook, so don't need to call getters again
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
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset Common::lotZPos
            push offset Common::lotZPosParam
            call [edx+0x34]
            jmp RegisterCanvasMaterialDefinition_Exit
        }
    }
}