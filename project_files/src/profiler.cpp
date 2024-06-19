
#include "profiler.hpp"

/**
 * @brief Sets up threads and timing for profilier.
 *
 */
Profiler::Profiler() {
    main_thread = OpenThread(
        THREAD_SUSPEND_RESUME |
            THREAD_GET_CONTEXT |
            THREAD_QUERY_INFORMATION,
        0,
        GetCurrentThreadId() );

    start = std::chrono::steady_clock::now();
    t1 = std::thread( &Profiler::Record, this );
}

/**
 * @brief Searches for occurance of function in function list.
 *
 * @param functionList List of identified functions
 * @param name         Name of function to search
 * @param nameLen      Length of name
 * @return int         Location of function in list (-1 if not found)
 */
int Profiler::find( std::vector< std::tuple< DWORD64, char*, int > >& functionList, char* name, int nameLen ) {
    // Looking through list for function name
    for ( int i = 0; i < functionList.size(); ++i ) {
        char* currentItemName = std::get< 1 >( functionList[i] );
        bool isSame = true;

        // Comparing names
        for ( int j = 0; j < nameLen; ++j ) {
            if ( currentItemName[j] != name[j] ) {
                isSame = false;
                break;
            }
        }
        if ( isSame )
            return i;
    }
    return -1;
}

/**
 * @brief Closes thread if still running function. Gets Symbol info and prints
 *        to file.
 *
 */
Profiler::~Profiler() {
    // Closing thread
    exit = true;
    t1.join();

    StartFile();

    // Setting up symbol information
    SymSetOptions( SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS );
    if ( !SymInitialize( GetCurrentProcess(), NULL, true ) ) {
        return;
    }

    // Condensing common items to new list
    std::vector< std::tuple< DWORD64, char*, int > > functionList;
    for ( int i = 0; i < eip_list.size(); ++i ) {
        ULONG64 buff[( sizeof( SYMBOL_INFO ) + MAX_SYM_NAME * sizeof( TCHAR ) + sizeof( ULONG64 ) - 1 ) / sizeof( ULONG64 )];
        PSYMBOL_INFO symbols = GetSymbol( eip_list[i].Rip, ( PSYMBOL_INFO )buff );

        size_t NameLen = symbols->NameLen;
        char* name = new char[NameLen + 3];
        name[0] = '"';
        for ( size_t j = 1; j < NameLen + 1; ++j ) {
            name[j] = *( symbols->Name + j - 1 );
        }
        name[symbols->NameLen + 1] = '"';
        name[symbols->NameLen + 2] = '\0';

        // Checking if it's new function
        int result = find( functionList, name, symbols->NameLen + 3 );
        if ( result == -1 ) {

            functionList.push_back( std::make_tuple( eip_list[i].Rip, name, 1 ) );
        } else {
            std::get< 2 >( functionList[result] ) += 1;
            delete[] name;
        }
    }

    // Writing output file
    for ( int i = 0; i < functionList.size(); ++i ) {
        WriteMessage( functionList[i] );
        delete[] std::get< 1 >( functionList[i] );
    }

    CloseFile();
}

/**
 * @brief Recording EIP every 1 ms
 *
 */
void Profiler::Record() {
    while ( true ) {
        if ( exit )
            return;

        // Checking time since last recorded
        current = std::chrono::steady_clock::now();
        long long duration = std::chrono::duration_cast< std::chrono::milliseconds >( current - start ).count();
        if ( duration < 1 )
            continue;

        SuspendThread( main_thread );

        // Getting info
        CONTEXT context = { 0 };
        context.ContextFlags = WOW64_CONTEXT_i386 | CONTEXT_CONTROL;
        GetThreadContext( main_thread, &context );

        ResumeThread( main_thread );

        // Saving to list and checking if max samples have been gotten
        eip_list.push_back( context );
        if ( eip_list.size() >= 100000 ) {
            return;
        }

        start = std::chrono::steady_clock::now();
    }
}

/**
 * @brief Getting symbol info from EIP
 *
 * @param address EIP
 * @param buff    Buffer for symbol information
 * @return PSYMBOL_INFO
 */
PSYMBOL_INFO Profiler::GetSymbol( DWORD64 address, PSYMBOL_INFO buff ) {
    PDWORD64 displacement = 0;
    PSYMBOL_INFO symbol = ( PSYMBOL_INFO )buff;
    symbol->SizeOfStruct = sizeof( SYMBOL_INFO );
    symbol->MaxNameLen = MAX_SYM_NAME;
    SymFromAddr( GetCurrentProcess(), address, displacement, symbol );
    return symbol;
}

/**
 * @brief Setting up output file.
 *
 */
void Profiler::StartFile() {
    // Checking if file is already open
    if ( hasWritten )
        return;

    // Opening file and setting csv headers
    int err = fopen_s( &( debug_file ), "ProfileReport.csv", "w+" );
    if ( err != 0 ) {
        printf( "Failed to open file." );
        return;
    }

    fprintf( debug_file, "Function, Hit Count, Percentage\n" );
    hasWritten = true;
}

/**
 * @brief Closes output file
 *
 */
void Profiler::CloseFile() {
    errno_t err = fclose( debug_file );
    if ( err != 0 )
        printf( "Failed to close file." );
}

void Profiler::WriteMessage( std::tuple< DWORD64, char*, int >& functionInfo ) {
    float hitCount = ( float )std::get< 2 >( functionInfo );
    float totalHits = ( float )eip_list.size();
    float hitPercentage = ( hitCount / totalHits ) * 100.f;

    fprintf( debug_file, "%s,%i,%f%%\n",
             std::get< 1 >( functionInfo ),
             std::get< 2 >( functionInfo ),
             hitPercentage );
}