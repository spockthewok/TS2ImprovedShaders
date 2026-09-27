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
    static void GetLotScale(char *lotAxis, const DWORD addrOffset)
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
                    Common::SetParamInt(lotAxis, Common::intMax, addr);
                    return;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        // RPCLib returns lotXScale/lotYScale regardless
        Common::SetParamInt(lotAxis, Common::intMax, 0);
    }

    static void GetIsBeachLotFromStr(const char *lotTemplate)
    {
        bool isBeach;

        if (!lotTemplate)
            isBeach = false;
        else
            // Beaches will either be "BeachCommunityLotTemplate" or "BeachLotTemplate"
            isBeach = (_strnicmp(lotTemplate, "Beach", 5) == 0);

        Common::SetParamBool(isBeachLot, Common::boolMax, isBeach);
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
    static float __declspec(naked) GetLotZPos()
    {
        __asm {
            call TS::Globals
            mov [esp+0x48],eax
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x5C] // cTSGlobals::GameStateController
            test eax,eax
            jz LAB_Null
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x24] // cTSGameStateController::CurrentLotInfo
            test eax,eax
            jz LAB_Null
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x40] // cTSLotInfo::NHoodToLotHeightOffset
            mov eax,[esp+0x48]
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x2C] // cTSGlobals::NeighborhoodTerrain
            test eax,eax
            jz LAB_Null
            mov edx,[eax]
            mov ecx,eax
            call [edx+0x40] // cTSNHoodTerrain::SeaLevel
            fsubp st(1),st(0) // lotZPos = lotHeightOffset - seaLevel
            jmp LAB_Return
        LAB_Null:
            fldz
        LAB_Return:
            ret
        }
    }

    static void InitTerrainParams()
    {
        GetLotScale(lotXScale, 0x64);
        GetLotScale(lotYScale, 0x68);
        Common::SetParamFloat(Common::lotZPos, Common::floatMax, GetLotZPos());
    }

    static void RegisterTerrainParams(nRZSceneGraph::cMaterialDefinition *matDef)
    {
        matDef->SetParameter(lotXScaleParam, lotXScale);
        matDef->SetParameter(lotYScaleParam, lotYScale);
        matDef->SetParameter(isBeachLotParam, isBeachLot);
        matDef->SetParameter(Common::lotZPosParam, Common::lotZPos);
    }

    // cTerrain::RegisterPaintMaterialDefinition
    void __declspec(naked) AddTerrainPaintParams()
    {
        __asm {
            push 0x123EAA4 // "alphaMapScaleV"
            call [eax+0x34]
            pushad
            call InitTerrainParams
            popad
            push [esp+0x14]
            call RegisterTerrainParams
            add esp,0x4
            jmp RegisterPaintMaterialDefinition_Exit
        }
    }

    // cTerrain::RegisterCanvasMaterialDefinition
    void __declspec(naked) AddTerrainCanvasParams()
    {
        __asm {
            push 0x123EB5C // "texture"
            call [edx+0x34]
            push [esp+0x18]
            call RegisterTerrainParams
            add esp,0x4
            jmp RegisterCanvasMaterialDefinition_Exit
        }
    }
}