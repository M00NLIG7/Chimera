#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>

#define SERVER_PORT 12345
// #define SERVER_IP "127.0.0.1"

// char *get_system_info();
void evil_fetch(char *ip);
#endif /* CLIENT_H */
