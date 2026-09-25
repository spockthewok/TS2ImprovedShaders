#include "lotskirt.h"
#include "TS2.h"
#include "common.h"

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;

    const size_t paramMax = 11;

    // Distance between lot z and sea level
    const char lotZPosParam[] = "lotZPos";
    char lotZPos[paramMax];

    // Lot x/y offset from world (0, 0)
    const char lotXOffsetParam[] = "lotXOffset";
    char lotXOffset[paramMax];
    const char lotYOffsetParam[] = "lotYOffset";
    char lotYOffset[paramMax];

    const char *roadMaterial = nullptr;
}

namespace LotSkirt
{
    static void SetLotZPosParam(const float seaZPos)
    {
        // seaZPos = sea level relative to lot z
        // Negate seaZPos to get lot z relative to sea level
        Common::SetParamValue(lotZPos, sizeof(lotZPos), -seaZPos);
    }

    static void SetLotOffsetParams(const int offsetX, const int offsetY)
    {
        // Could leave these as ints but the params are floats in Castaway Stories
        Common::SetParamValue(lotXOffset, sizeof(lotXOffset), static_cast<float>(offsetX));
        Common::SetParamValue(lotYOffset, sizeof(lotYOffset), static_cast<float>(offsetY));
    }

    // All param values are object vars precalculated by cLotSkirt::ComputeLotSkirtParameters
    // We skip calling setters for roads as values will have already been set by lot skirt material
    static void __declspec(naked) AddNewParams()
    {
        __asm {
            cmp [roadMaterial],0x0
            jne LAB_SkipSetZPos
            push [edi+0xBC]
            call SetLotZPosParam
            add esp,0x4
        LAB_SkipSetZPos:
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotZPos
            push offset lotZPosParam
            call [edx+0x34]
            cmp [roadMaterial],0x0
            jne LAB_SkipSetOffsets
            push [edi+0xFC]
            push [edi+0xF8]
            call SetLotOffsetParams
            add esp,0x8
        LAB_SkipSetOffsets:
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotXOffset
            push offset lotXOffsetParam
            call [edx+0x34]
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset lotYOffset
            push offset lotYOffsetParam
            call [edx+0x34]
            ret
        }
    }

    static const char *GetRoadTextureName(const char *matName)
    {
        if (!matName)
            return "";

        std::string roadTexture(matName);

        // All vanilla road texture names are same as material name, but with '4' as last char
        // Don't think there are any mods that add new road types which might break this rule
        if (!roadTexture.empty())
            roadTexture.back() = '4';

        return roadTexture.c_str();
    }

    // cLotSkirt::RegisterMaterials
    // Lot skirt road material doesn't normally have code-based params
    // This hijacks cLotSkirt::RegisterMaterials and provides a path for adding params to roads
    // Not sold on this approach, but it's simpler than trying to init a cMaterialDefinition object
    void __declspec(naked) HandleRoadParams()
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
            push 0x1241F54 // "LotSkirtRoadMaterialDefinition"
            jmp LAB_SetDefinition
        LAB_LotSkirtDefinition:
            push 0x123AFDC // "LotSkirtMaterialDefinition"
        LAB_SetDefinition:
            call [eax+0x30]
            cmp [roadMaterial],0x0
            je LAB_LotSkirtTexture
            push [roadMaterial]
            call GetRoadTextureName
            add esp,0x4
            jmp LAB_SetTextureParam
        LAB_LotSkirtTexture:
            mov eax,[esp+0x30]
        LAB_SetTextureParam:
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push eax
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            call AddNewParams
            jmp RegisterMaterials_Exit
        }
    }

    // cLotSkirt::CreateNodesForRoadCells
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
}