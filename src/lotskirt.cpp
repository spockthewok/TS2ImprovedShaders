#include "lotskirt.h"
#include "TS2/base.h"
#include "common.h"
#include <string>

namespace
{
    const DWORD RegisterMaterials_Exit = 0xA83775;
    const DWORD CreateNodesForRoadCells_Exit = 0xA8432F;
}

namespace LotSkirt
{
    static void RegisterSkirtParams(nRZSceneGraph::cMaterialDefinition *matDef)
    {
        matDef->SetParameter("lotXOffset", Common::lotXOffset);
        matDef->SetParameter("lotYOffset", Common::lotYOffset);
        matDef->SetParameter("lotZPos", Common::lotZPos);
    }

    // cLotSkirt::RegisterMaterials
    void __declspec(naked) AddLotSkirtParams()
    {
        __asm {
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            push [esp+0x14]
            call RegisterSkirtParams
            add esp,0x4
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

        if (!matDef)
            return;

        if (!matMgr || !matName)
        {
            matDef->Release();
            return;
        }

        matDef->SetMaterialName(matName);
        matDef->SetDefinition("LotSkirtRoadMaterialDefinition");
        matDef->SetParameter("surfaceTexture", GetRoadTextureName(matName));
        matDef->SetParameter("lotZPos", Common::lotZPos);
        matMgr->RegisterMaterialDefinition(matDef, 0);
        matDef->Release();
    }

    // cLotSkirt::CreateNodesForRoadCells
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