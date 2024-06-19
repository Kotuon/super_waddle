
#ifndef CRASH_HANDLER
#define CRASH_HANDLER

#include <Windows.h>
#include <dbghelp.h>

#pragma comment( lib, "dbghelp.lib" )

LONG WINAPI WriteDump( EXCEPTION_POINTERS* pException );
void SetupDump();

#endif CRASH_HANDLER
