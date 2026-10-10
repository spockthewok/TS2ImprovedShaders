#pragma once
#include "headers.h"
#include "TS2/cTSBaseLotInfo.h"

class cTSLotInfo
{
public:
    cTSBaseLotInfo *GetBaseLotInfo();
    void GetLocation(ULONG32 *x, ULONG32 *y);
    float NHoodToLotHeightOffset();
    BYTE RotationRelativeToCreation();
};