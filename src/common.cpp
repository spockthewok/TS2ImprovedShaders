#include "common.h"
#include <string>

namespace Common
{
    // Lot z relative to sea level
    const char lotZPosParam[] = "lotZPos";
    char lotZPos[floatMax];

    template <typename T>
    static void SetParamImpl(char *param, size_t paramSize, const T &value, bool asFloat = false)
    {
        std::string valueStr;

        if constexpr (std::is_same_v<T, std::string>)
            valueStr = value;
        else if constexpr (std::is_same_v<T, bool>)
            valueStr = value ? "true" : "false";
        else if ((constexpr(std::is_same_v<T, int>)) && asFloat)
            valueStr = std::to_string(static_cast<float>(value));
        else
            valueStr = std::to_string(value);

        strcpy_s(param, paramSize, valueStr.c_str());
    }

    void SetParamInt(char *param, size_t paramSize, const int value, bool asFloat)
    {
        SetParamImpl(param, paramSize, value, asFloat);
    }

    void SetParamFloat(char *param, size_t paramSize, const float value)
    {
        SetParamImpl(param, paramSize, value);
    }

    void SetParamBool(char *param, size_t paramSize, const bool value)
    {
        SetParamImpl(param, paramSize, value);
    }
}