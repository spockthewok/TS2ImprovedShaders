#pragma once
#include <string>

namespace Common
{
    template <typename T>
    void SetParamValue(char *param, size_t paramSize, const T &value)
    {
        std::string valueStr;

        if constexpr (std::is_same_v<T, std::string>)
            valueStr = value;
        else if constexpr (std::is_same_v<T, bool>)
        {
            if (value)
                valueStr = "true";
            else
                valueStr = "false";
        }
        else
            valueStr = std::to_string(value);

        strcpy_s(param, paramSize, valueStr.c_str());
    }
}