#pragma once
#include "headers.h"

class cGZCOM
{
public:
    template <typename T>
    void GetClassObject(DWORD objID1, DWORD objID2, T **object)
    {
        using Func = void(__thiscall *)(cGZCOM *, DWORD, DWORD, T **);
        auto vFunc = Common::GetVTableFn<Func>(this, 0xC);
        vFunc(this, objID1, objID2, object);
    }
};