// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment( lib, "Psapi.lib" )
#define DPSAPI_VERSION (1)

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <strsafe.h>
#include <tlhelp32.h>
#include <conio.h>
#include <psapi.h>


using namespace std;
#define BUF_SIZE 255



DWORD GetProcessIdByName(DWORD processId, TCHAR* sProcessNameWanted);
DWORD WINAPI Thread_1 (LPVOID lpParam);
void GetProcessBaseAddr(DWORD processId);




// TODO: reference additional headers your program requires here
