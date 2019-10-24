#pragma once
#include <Windows.h>
#include <string>
#include "Memory.h"
#include "Overlay.h"

extern int screenX;
extern int screenY;
extern int bSkl;
extern HANDLE GameHandle;
extern LPVOID BaseAddress;
extern Memory Mem;
extern Overlay Over;
void GetResolution(UINT horizontal, UINT vertical);