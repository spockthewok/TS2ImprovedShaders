#include "shaders.h"
#include "headers.h"

namespace
{
    const DWORD UpdateWeatherShaders_Exit = 0xB24C82;

    const char lotSkirtWater[] = "LotSkirtWater";
    const char nhoodBuildingMaterial[] = "NeighborhoodBuildingMaterial";
    // Requires Better Nightlife
    // https://www.tumblr.com/criquette-was-here/157941568866/better-nightlife-ts2-custom-hood-deco-night
    const char nhoodGlowMaterial[] = "NeighborhoodGlowMaterial";
}

namespace Shaders
{
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