#include "shaders.h"
#include "hooking.h"
#include "TS2.h"
#include <string>

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;
    const DWORD RegisterPaintMaterialDefinition_Exit = 0xAE23C2;
    const DWORD RegisterCanvasMaterialDefinition_Exit = 0xAE2ED0;
    const DWORD UpdateWeatherShaders_Exit = 0xB24C82;
    const DWORD LoadLot_Exit = 0xEFC9D0;
    const DWORD Load_Exit = 0x102C951;

    const size_t lotSizeMax = 11;

    // Lot width/height
    const char lotXScaleParam[] = "lotXScale";
    char lotXScale[lotSizeMax];
    const char lotYScaleParam[] = "lotYScale";
    char lotYScale[lotSizeMax];

    // Distance between lot z and sea level
    const char lotZPosParam[] = "lotZPos";
    char lotZPos[lotSizeMax];

    // Lot x/y offset from world (0, 0)
    const char lotXOffsetParam[] = "lotXOffset";
    char lotXOffset[lotSizeMax];
    const char lotYOffsetParam[] = "lotYOffset";
    char lotYOffset[lotSizeMax];

    const char isBeachLotParam[] = "isBeachLot";
    char isBeachLot[6];

    const char lotSkirtWater[] = "LotSkirtWater";
    const char nhoodBuildingMaterial[] = "NeighborhoodBuildingMaterial";
    // Requires Better Nightlife
    // https://www.tumblr.com/criquette-was-here/157941568866/better-nightlife-ts2-custom-hood-deco-night
    const char nhoodGlowMaterial[] = "NeighborhoodGlowMaterial";

    const char *roadMaterial = nullptr;
}

namespace Shaders
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
                    strcpy_s(lotAxis, lotSizeMax, std::to_string(addr).c_str());
                    return lotAxis;
                }
            }
        }
        // In case memory isn't readable for whatever reason
        // RPCLib returns lotXScale/lotYScale regardless
        strcpy_s(lotAxis, lotSizeMax, "0");
        return lotAxis;
    }

    // Parameter expects a string rather than a boolean
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

    static void SetLotZPosParam(const float seaZPos)
    {
        // Negate seaZPos to get lot z relative to sea level
        strcpy_s(lotZPos, sizeof(lotZPos), std::to_string(-seaZPos).c_str());
    }

    static void SetLotOffsetParams(const int offsetX, const int offsetY)
    {
        strcpy_s(lotXOffset, sizeof(lotXOffset), std::to_string((float)offsetX).c_str());
        strcpy_s(lotYOffset, sizeof(lotYOffset), std::to_string((float)offsetY).c_str());
    }

    static const char *SetRoadTextureName(const char *matName)
    {
        if (!matName)
            return "";

        std::string roadTexture(matName);
        roadTexture.back() = '4';

        return roadTexture.c_str();
    }

    // cLotSkirt::RegisterMaterials
    // This disgusting code adds extra parameters to lot skirt and road shaders
    void __declspec(naked) AddLotSkirtParams()
    {
        __asm {
            cmp [roadMaterial],0x0
            je LAB_LotSkirtMaterial
            mov eax,[roadMaterial]
            jmp LAB_SetMaterial
        LAB_LotSkirtMaterial:
            mov eax,[edi+0x7C]
        LAB_SetMaterial:
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax
            call [edx+0x28]
            mov ecx,[esp+0x14]
            mov eax,[ecx]
            cmp [roadMaterial],0x0
            je LAB_LotSkirtDefinition
            push 0x1241F54
            jmp LAB_SetDefinition
        LAB_LotSkirtDefinition:
            push 0x123AFDC
        LAB_SetDefinition:
            call [eax+0x30]
            cmp [roadMaterial],0x0
            je LAB_LotSkirtTexture
            push [roadMaterial]
            call SetRoadTextureName
            add esp,0x4
            jmp LAB_SetTexture
        LAB_LotSkirtTexture:
            mov eax,[esp+0x30]
        LAB_SetTexture:
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            push [edi+0xBC] // Sea level relative to lot z
            call SetLotZPosParam
            add esp,0x4
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotZPos
            push offset lotZPosParam
            call [edx+0x34]
            pushad
            push [edi+0xFC]
            push [edi+0xF8]
            call SetLotOffsetParams
            add esp,0x8
            popad
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotXOffset
            push offset lotXOffsetParam
            call [edx+0x34]
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotYOffset
            push offset lotYOffsetParam
            call [edx+0x34]
            jmp RegisterMaterials_Exit
        }
    }

    void __declspec(naked) AddLotSkirtRoadParams()
    {
        __asm {
            mov ecx,[esp+0x5C]
            mov [roadMaterial],ecx
            mov ecx,ebx
            call cLotSkirt::RegisterMaterials
            mov [roadMaterial],0x0
            mov eax,[esp+0x5C]
            cmp eax,edi
            jmp CreateNodesForRoadCells_Exit
        }
    }

    // cTSSGSystem::UpdateWeatherShaders
    // Adds additional materials that should be updated on time/weather/season change
    // Building and glow materials added to fix stuck lights when dawn/dusk states enabled
    void __declspec(naked) AddWeatherShaderMaterials()
    {
        __asm {
            push 0x123EDB0 // "TerrainCanvasShader"
            mov ecx,esi
            call [eax+0x78]
            mov edx,[esi]
            push 0x123E920 // "NeighborhoodWater"
            mov ecx,esi
            call [edx+0x78]
            mov eax,[esi]
            push 0x1241F08 // "TerrainWater"
            mov ecx,esi
            call [eax+0x78]
            mov edx,[esi]
            push offset lotSkirtWater
            mov ecx,esi
            call [edx+0x78]
            mov eax,[esi]
            push offset nhoodBuildingMaterial
            mov ecx,esi
            call [eax+0x78]
            mov edx,[esi]
            push offset nhoodGlowMaterial
            mov ecx,esi
            call [edx+0x78]
            mov eax,[esi]
            push 0x123AFDC // "LotSkirtMaterialDefinition"
            jmp UpdateWeatherShaders_Exit
        }
    }
}