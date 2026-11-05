#include "terrain.h"
#include "hooking.h"
#include "TS2/base.h"
#include "common.h"

namespace
{
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit_1 = 0xAE2ED0;
    const DWORD RegisterCanvasMaterialDefinition_Exit_2 = 0xAE2F61;
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

    static void GetLotScale(int *scale)
    {
        nTSWorld::cWorldDB *worldDB = TS::WorldDB();

        if (!worldDB)
            return;

        scale[0] = worldDB->NumTilesX();
        scale[1] = worldDB->NumTilesY();
    }

    // This does very similar calculations to cLotSkirt::ComputeLotSkirtParameters
    // Terrain materials are set up before lot skirt, so we use our own function
    static float GetLotZPos()
    {
        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!lotInfo)
            return 0.0f;

        cTSGlobals *globals = TS::Globals();
        cTSNHoodTerrain *nhoodTerrain = globals->NeighborhoodTerrain();

        if (!nhoodTerrain)
            return 0.0f;

        float lotHeightOffset = lotInfo->NHoodToLotHeightOffset();
        float seaLevel = nhoodTerrain->SeaLevel();

        return lotHeightOffset - seaLevel;
    }

    // This is near identical to cLotSkirt::ComputeNHoodToLotTransformationParameters
    // Again, we use our own function as calling the lot skirt method early can cause crashes
    static void GetLotXYOffsets(ULONG32 *offsets)
    {
        ULONG32 size[2];
        ULONG32 location[2];
        ULONG32 width = 0;
        ULONG32 height = 0;

        cTSGameStateController *gameState = TS::GameStateController();
        cTSLotInfo *lotInfo = gameState->CurrentLotInfo();

        if (!lotInfo)
            return;

        UINT rotation = lotInfo->RotationRelativeToCreation() & 0xFF;
        cTSBaseLotInfo *baseLotInfo = lotInfo->GetBaseLotInfo();
        baseLotInfo->GetSizeAtCreation(&size[0], &size[1]);
        lotInfo->GetLocation(&location[0], &location[1]);

        switch (rotation)
        {
        case 1:
            height = size[0];
            break;
        case 2:
            width = size[0];
            height = size[1];
            break;
        case 3:
            width = size[1];
        }

        offsets[0] = width + location[0];
        offsets[1] = height + location[1];
    }

    static void InitTerrainParams()
    {
        int scale[2] = {0, 0};
        ULONG32 offsets[2] = {0, 0};

        GetLotScale(scale);
        GetLotXYOffsets(offsets);

        Common::SetParamInt(lotXScale, Common::intMax, scale[0]);
        Common::SetParamInt(lotYScale, Common::intMax, scale[1]);
        Common::SetParamFloat(Common::lotZPos, Common::floatMax, GetLotZPos());
        Common::SetParamInt(Common::lotXOffset, Common::floatMax, offsets[0], true);
        Common::SetParamInt(Common::lotYOffset, Common::floatMax, offsets[1], true);
        paramsSet = true;
    }

    static void RegisterTerrainParams(nRZSceneGraph::cMaterialDefinition *matDef)
    {
        matDef->SetParameter("lotXScale", lotXScale);
        matDef->SetParameter("lotYScale", lotYScale);
        matDef->SetParameter("lotXOffset", Common::lotXOffset);
        matDef->SetParameter("lotYOffset", Common::lotYOffset);
        matDef->SetParameter("lotZPos", Common::lotZPos);
        matDef->SetParameter("isBeachLot", isBeachLot);
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
            jmp RegisterCanvasMaterialDefinition_Exit_1
        }
    }

    // cTerrain::RegisterCanvasMaterialDefinition
    void __declspec(naked) AddTerrainCutoutParams()
    {
        __asm {
            push 0x123EDA4 // "hasCutout"
            call [eax+0x34]
            push [esp+0x14]
            call RegisterTerrainParams
            add esp,0x4
            mov [paramsSet],0x0
            jmp RegisterCanvasMaterialDefinition_Exit_2
        }
    }
}