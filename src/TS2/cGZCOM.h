#pragma once
#include "headers.h"

class cGZCOM
{
public:
    template <typename T>
    void GetClassObject(DWORD classID, DWORD interfaceID, T **object)
    {
        using Func = void(__thiscall *)(cGZCOM *, DWORD, DWORD, T **);
        auto vFunc = Common::GetVTableFn<Func>(this, 0xC);
        vFunc(this, classID, interfaceID, object);
    }
};