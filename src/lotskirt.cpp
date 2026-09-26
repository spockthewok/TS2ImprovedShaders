#include "lotskirt.h"
#include "TS2.h"
#include "common.h"
#include <string>

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;

    // Lot x/y offset from world (0, 0)
    const char lotXOffsetParam[] = "lotXOffset";
    char lotXOffset[Common::floatMax];
    const char lotYOffsetParam[] = "lotYOffset";
    char lotYOffset[Common::floatMax];

    const char *roadMaterial = nullptr;
}

namespace LotSkirt
{
    // We skip calling setters for roads as values will have been set by lot skirt material
    static void __declspec(naked) AddNewParams()
    {
        __asm {
            mov ecx,[esp+0x18]
            mov edx,[ecx]
            push offset Common::lotZPos
            push offset Common::lotZPosParam
            call [edx+0x34]
            cmp [roadMaterial],0x0
            jne LAB_SkipSetOffsets
            push 0x1 // asFloat = true
            push [edi+0xF8]
            push [Common::floatMax]
            push offset lotXOffset
            call Common::SetParamInt
            add esp,0x10
            push 0x1 // asFloat = true
            push [edi+0xFC]
            push [Common::floatMax]
            push offset lotYOffset
            call Common::SetParamInt
            add esp,0x10
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