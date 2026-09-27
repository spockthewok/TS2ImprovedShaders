#include "lotskirt.h"
#include "TS2/base.h"
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
}

namespace LotSkirt
{
    static void RegisterSkirtParams(nRZSceneGraph::cMaterialDefinition *matDef, int xOff, int yOff)
    {
        Common::SetParamInt(lotXOffset, Common::floatMax, xOff, true);
        Common::SetParamInt(lotYOffset, Common::floatMax, yOff, true);
        matDef->SetParameter(lotXOffsetParam, lotXOffset);
        matDef->SetParameter(lotYOffsetParam, lotYOffset);
        matDef->SetParameter(Common::lotZPosParam, Common::lotZPos);
    }

    // cLotSkirt::RegisterMaterials
    // X/Y offset params use object vars precalculated by cLotSkirt::ComputeLotSkirtParameters
    void __declspec(naked) AddLotSkirtParams()
    {
        __asm {
            push 0x123AFCC // "surfaceTexture"
            call [edx+0x34]
            push [edi+0xFC]
            push [edi+0xF8]
            push [esp+0x1C]
            call RegisterSkirtParams
            add esp,0xC
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