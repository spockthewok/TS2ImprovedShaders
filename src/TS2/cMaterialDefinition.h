#pragma once

namespace nRZSceneGraph
{
    class cMaterialDefinition
    {
    public:
        void Release();
        void SetMaterialName(const char *matName);
        void SetDefinition(const char *matDef);
        void SetParameter(const char *param, const char *value);
    };
}