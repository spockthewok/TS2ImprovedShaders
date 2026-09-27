#pragma once
#include "TS2/cMaterialDefinition.h"

namespace nRZSceneGraph
{
    class cMaterialManager
    {
    public:
        void RegisterMaterialDefinition(cMaterialDefinition *matDef, int flag);
    };
}