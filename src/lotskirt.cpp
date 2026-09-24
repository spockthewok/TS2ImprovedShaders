#include "lotskirt.h"
#include "TS2.h"
#include <string>

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;

    const size_t lotSizeMax = 11;

    // Distance between lot z and sea level
    const char lotZPosParam[] = "lotZPos";
    char lotZPos[lotSizeMax];

    // Lot x/y offset from world (0, 0)
    const char lotXOffsetParam[] = "lotXOffset";
    char lotXOffset[lotSizeMax];
    const char lotYOffsetParam[] = "lotYOffset";
    char lotYOffset[lotSizeMax];

    const char *roadMaterial = nullptr;
}

namespace LotSkirt
{
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
    // If road material name is null then adds params to lot skirt, otherwise adds to roads
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
            push 0x1241F54 // "LotSkirtRoadMaterialDefinition"
            jmp LAB_SetDefinition
        LAB_LotSkirtDefinition:
            push 0x123AFDC // "LotSkirtMaterialDefinition"
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