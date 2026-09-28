#pragma once
#include "headers.h"
#include "TS2/cMaterialDefinition.h"

namespace Common
{
    // These are global vars to make passing array size to setters using ASM easier
    // intMax may seem redundant, but it helps to distinguish param types
    constexpr size_t floatMax = 11;
    constexpr size_t intMax = floatMax;
    constexpr size_t boolMax = 6;

    extern char lotZPos[floatMax];

    template <typename Fn, typename T>
    Fn GetVTableFn(T *object, const DWORD offset)
    {
        constexpr size_t indexSize = sizeof(uintptr_t);
        auto vTable = *reinterpret_cast<uintptr_t **>(object);

        return reinterpret_cast<Fn>(vTable[offset / indexSize]);
    }

    // These are wrappers for calling SetParamImpl()
    // Wish I didn't need them, but it's not possible to call template funcs directly using ASM :/
    void SetParamInt(char *param, size_t paramSize, const int value, bool asFloat = false);
    void SetParamFloat(char *param, size_t paramSize, const float value);
    void SetParamBool(char *param, size_t paramSize, const bool value);

    nRZSceneGraph::cMaterialDefinition *InitMaterialDefinition();
}