#include <windows.h>
#include <stdio.h>

const int WRONG_INPUT_EXIT_CODE=101;
const int RUN_ERROR_EXIT_CODE=102;
const int TIMEOUT_EXIT_CODE=103;

void KillProcessById(DWORD pid) {
    HANDLE hnd;
    hnd = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
    TerminateProcess(hnd, 0);
}

int run(char* command, int ms) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( !CreateProcess( NULL,   // No module name (use command line)
        command,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return RUN_ERROR_EXIT_CODE;
    }

    // Wait until child process exits.
    DWORD exit_code;
    DWORD rtn = WaitForSingleObject( pi.hProcess, ms);
    if (rtn == WAIT_TIMEOUT) {
        KillProcessById(pi.dwProcessId);
        exit_code = TIMEOUT_EXIT_CODE;
    } else {
        GetExitCodeProcess(pi.hProcess, &exit_code);
    }
    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return exit_code;
}

void help() {
    printf("%s", "Usage: timeout milliseconds command\n");
    printf("%s", "Author: https://helloacm.com");
}

int main(int argc, char** argv) {
    if (argc == 3) {
        int ms = atoi(argv[1]);
        char* command = argv[2];
        int exit_code = run(command, ms);
        return exit_code;
    } else {
        help();
        return WRONG_INPUT_EXIT_CODE;
    }
}
