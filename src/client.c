// #include "client.h"
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

char *get_system_info() {
  struct sysinfo sys_info;
  sysinfo(&sys_info);

  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  struct utsname uname_info;
  uname(&uname_info);

  int cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
  unsigned long total_memory = sys_info.totalram / 1024 / 1024;
  unsigned long used_memory = (sys_info.totalram - sys_info.freeram) / 1024 / 1024;
  unsigned long free_memory = sys_info.freeram / 1024 / 1024;

  struct statvfs vfs;
  statvfs("/", &vfs);

  unsigned long disk_total = (vfs.f_blocks * vfs.f_frsize) / (1024 * 1024);
  unsigned long disk_free = (vfs.f_bfree * vfs.f_frsize) / (1024 * 1024);
  unsigned long disk_used = disk_total - disk_free;
  unsigned long disk_available = (vfs.f_bavail * vfs.f_frsize) / (1024 * 1024);

  char *json_data = (char *)malloc(1024 * sizeof(char));
  snprintf(json_data, 1024,
           "{"
           "\"hostname\": \"%s\","
           "\"cpu\": {\"model\": \"%s\", \"cores\": %d},"
           "\"memory\": {\"total\": %lu, \"used\": %lu, \"free\": %lu},"
           "\"disk\": {\"total\": %lu, \"used\": %lu, \"available\": %lu}"
           "}",
           hostname, uname_info.machine, cpu_cores, total_memory, used_memory, free_memory,
           disk_total, disk_used, disk_available);

  return json_data;
}

void evil_fetch(char *ip) {
  int sockfd;
  struct sockaddr_in serv_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, ip, &serv_addr.sin_addr);

  connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  char *json_data = get_system_info();

  send(sockfd, json_data, strlen(json_data), 0);

  printf("%s", json_data);
  free(json_data);
  close(sockfd);
}
