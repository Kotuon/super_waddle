
#include "crash_handler.hpp"

LONG WINAPI WriteDump( EXCEPTION_POINTERS* pException ) {
    MINIDUMP_EXCEPTION_INFORMATION information = MINIDUMP_EXCEPTION_INFORMATION{ GetCurrentThreadId(), pException, FALSE };

    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        CreateFileA( "crash_mini.dmp", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ),
        MINIDUMP_TYPE( MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithCodeSegs | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithUnloadedModules | MiniDumpWithFullMemory ),
        &information,
        NULL,
        NULL );

    return EXCEPTION_CONTINUE_SEARCH;
}

void SetupDump() {
    ULONG guaranteedStackSize = 17000;
    SetUnhandledExceptionFilter( WriteDump );
    SetThreadStackGuarantee( &guaranteedStackSize );
}
