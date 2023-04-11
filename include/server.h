#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024



typedef struct {
    int sockfd;
    struct sockaddr_in cli_addr;
} connection_t;

void start_server();

#endif /* SERVER_H */
