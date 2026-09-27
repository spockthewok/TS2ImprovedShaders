#include "hooking.h"

namespace Hooking
{
    void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen)
    {
        DWORD dwOldProtect, dwBkup, dwRelAddr;

        VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
        dwRelAddr = (DWORD)(dwJumpTo - (DWORD)pAddress) - 5;
        *pAddress = 0xE9;
        *((DWORD *)(pAddress + 0x1)) = dwRelAddr;

        for (DWORD x = 0x5; x < dwLen; x++)
            *(pAddress + x) = 0x90;

        VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);

        return;
    }

    bool MemoryReadable(const void *ptr, size_t byteCount)
    {
        MEMORY_BASIC_INFORMATION mbi;

        if (VirtualQuery(ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
            return false;

        if (mbi.State != MEM_COMMIT)
            return false;

        if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
            return false;

        size_t blockOffset = (size_t)((char *)ptr - (char *)mbi.AllocationBase);
        size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

        if (blockBytesPostPtr < byteCount)
            return MemoryReadable((char *)ptr + blockBytesPostPtr, byteCount - blockBytesPostPtr);

        return true;
    }
}