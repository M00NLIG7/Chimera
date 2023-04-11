#include "server.h"

void *handle_client(void *arg) {
    connection_t *conn = (connection_t *) arg;
    int newsockfd = conn->sockfd;
    struct sockaddr_in cli_addr = conn->cli_addr;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    recv(newsockfd, buffer, BUFFER_SIZE - 1, 0);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    char hostname[32];
    char cpu_model[32];
    int cpu_cores;
    int memory_total, memory_used, memory_free;
    int disk_total, disk_used, disk_available;

    sscanf(buffer, "{\"hostname\": \" %31[^\"] \",\"cpu\": {\"model\": \" %31[^\"] \", \"cores\": %d },\"memory\": {\"total\": %d , \"used\": %d , \"free\": %d },\"disk\": {\"total\": %d , \"used\": %d , \"available\": %d }}",
        hostname, cpu_model, &cpu_cores, &memory_total, &memory_used, &memory_free, &disk_total, &disk_used, &disk_available);

    printf("Client IP: %s\n", client_ip);
    printf("Hostname: %s\n", hostname);
    printf("CPU Model: %s\n", cpu_model);
    printf("CPU Cores: %d\n", cpu_cores);
    printf("Memory Total: %d MB\n", memory_total);
    printf("Memory Used: %d MB\n", memory_used);
    printf("Memory Free: %d MB\n", memory_free);
    printf("Disk Total: %d MB\n", disk_total);
    printf("Disk Used: %d MB\n", disk_used);
    printf("Disk Available: %d MB\n", disk_available);
    printf("-----------------------------\n");

    close(newsockfd);
    pthread_exit(NULL);
}

void start_server() {
    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t clilen;
    pthread_t tid;
    connection_t *conn;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Set socket to non-blocking mode
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);
    clilen = sizeof(struct sockaddr_in);

    while (1) {
      conn = (connection_t *) malloc(sizeof(connection_t));
      int newsockfd = accept(sockfd, (struct sockaddr *)&conn->cli_addr, &clilen);
      if (newsockfd < 0) {
          // Free the memory allocated for the conn variable
          free(conn);
          if (errno == EWOULDBLOCK) {
              // No pending connections, sleep for a while
              usleep(1000);
              continue;
          } else {
              perror("accept");
              break;
          }
      }
      conn->sockfd = newsockfd;
      pthread_create(&tid, NULL, handle_client, (void *) conn);
      pthread_detach(tid);
    }

    close(sockfd);
}
