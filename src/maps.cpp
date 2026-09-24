#include "maps.h"
#include "hooking.h"

namespace Maps
{
    // cTSSGSystem::IsSC4ValidForImport
    // Removes the cImportSC4::IsSmallCity check, allowing medium/large SC4 maps to be imported
    void RemoveMapSizeLimit()
    {
        Hooking::Nop((BYTE *)0xB2675C, 57);
    }
}