#include "lotskirt.h"
#include "TS2/base.h"
#include "common.h"
#include <string>

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83772;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;

    // Lot x/y offset from world (0, 0)
    const char lotXOffsetParam[] = "lotXOffset";
    char lotXOffset[Common::floatMax];
    const char lotYOffsetParam[] = "lotYOffset";
    char lotYOffset[Common::floatMax];
}

namespace LotSkirt
{
    // cLotSkirt::RegisterMaterials
    // Adds extra parameters to lot skirt material
    // X/Y offset params use object vars precalculated by cLotSkirt::ComputeLotSkirtParameters
    void __declspec(naked) AddLotSkirtParams()
    {
        __asm {
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset Common::lotZPos
            push offset Common::lotZPosParam
            call [edx+0x34]
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
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotXOffset
            push offset lotXOffsetParam
            call [edx+0x34]
            mov ecx,[esp+0x14]
            mov edx,[ecx]
            push offset lotYOffset
            push offset lotYOffsetParam
            jmp RegisterMaterials_Exit
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

    static void RegisterRoadMaterials(const char *matName)
    {
        nRZSceneGraph::cMaterialManager *matMgr = nRZSceneGraph::MaterialManager();
        nRZSceneGraph::cMaterialDefinition *matDef = Common::InitMaterialDefinition();

        if (!matMgr || !matDef)
            return;

        matDef->SetMaterialName(matName);
        matDef->SetDefinition("LotSkirtRoadMaterialDefinition");
        matDef->SetParameter("surfaceTexture", GetRoadTextureName(matName));
        matDef->SetParameter(Common::lotZPosParam, Common::lotZPos);
        matMgr->RegisterMaterialDefinition(matDef, 0);
        matDef->Release();
    }

    // cLotSkirt::CreateNodesForRoadCells
    // Adds extra parameters to lot skirt road material
    void __declspec(naked) AddLotSkirtRoadParams()
    {
        __asm {
            push [esp+0x5C] // Road material name
            call RegisterRoadMaterials
            add esp,0x4
            mov eax,[esp+0x5C]
            cmp eax,edi
            jmp CreateNodesForRoadCells_Exit
        }
    }
}