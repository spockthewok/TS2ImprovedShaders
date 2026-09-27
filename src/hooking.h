#pragma once
#include "headers.h"

namespace Hooking
{
	void MakeJMP(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen);
	bool MemoryReadable(const void *ptr, size_t byteCount);
}