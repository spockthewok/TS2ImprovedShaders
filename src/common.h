#pragma once

namespace Common
{
    // These are global vars to make passing array size to setters using ASM easier
    // intMax may seem redundant, but it helps to distinguish param types
    constexpr size_t floatMax = 11;
    constexpr size_t intMax = floatMax;
    constexpr size_t boolMax = 6;

    extern const char lotZPosParam[];
    extern char lotZPos[floatMax];

    // These are wrappers for calling SetParamImpl()
    // Wish I didn't need these, but not possible to call template funcs directly using ASM :/
    void SetParamInt(char *param, size_t paramSize, const int value, bool asFloat = false);
    void SetParamFloat(char *param, size_t paramSize, const float value);
    void SetParamBool(char *param, size_t paramSize, const bool value);
}