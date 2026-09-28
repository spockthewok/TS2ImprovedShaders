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
    char lotXScale[Common::intMax];
    char lotYScale[Common::intMax];
    // Whether the current lot is a beach
    char isBeachLot[Common::boolMax];

    bool paramsSet = false;
}

namespace Terrain
{
    // Condensed version of TSGetLotXScale and TSGetLotYScale from RPCLib
    // https://github.com/LazyDuchess/RPCLib/blob/master/RPCLib/common.cpp
    static UINT GetLotScale(const BYTE addrOffset)
    {
        DWORD addr = 0x1478F10; // nTSSG::TSSGSystem
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
                    return addr;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        return 0;
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
    // We need this here as terrain materials are set up before lot skirt
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
        Common::SetParamInt(lotXScale, Common::intMax, GetLotScale(0x64));
        Common::SetParamInt(lotYScale, Common::intMax, GetLotScale(0x68));
        Common::SetParamFloat(Common::lotZPos, Common::floatMax, GetLotZPos());
        paramsSet = true;
    }

    static void RegisterTerrainParams(nRZSceneGraph::cMaterialDefinition *matDef)
    {
        matDef->SetParameter("lotXScale", lotXScale);
        matDef->SetParameter("lotYScale", lotYScale);
        matDef->SetParameter("isBeachLot", isBeachLot);
        matDef->SetParameter("lotZPos", Common::lotZPos);
    }

    // cTerrain::RegisterPaintMaterialDefinitions
    // This executes within a loop, so we make sure to only init params once per lot
    void __declspec(naked) AddTerrainPaintParams()
    {
        __asm {
            push 0x123EAA4 // "alphaMapScaleV"
            call [eax+0x34]
            cmp [paramsSet],0x0
            jne LAB_SkipInit
            call InitTerrainParams
        LAB_SkipInit:
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
            mov [paramsSet],0x0
            jmp RegisterCanvasMaterialDefinition_Exit
        }
    }
}