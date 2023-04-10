#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"

#include <sys/statvfs.h>
// ... (other includes)

char *get_system_info() {
  struct sysinfo sys_info;
  sysinfo(&sys_info);

  struct utsname uname_info;
  uname(&uname_info);

  int cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
  unsigned long total_memory = sys_info.totalram / 1024 / 1024;
  unsigned long used_memory = (sys_info.totalram - sys_info.freeram) / 1024 / 1024;
  unsigned long free_memory = sys_info.freeram / 1024 / 1024;

  struct statvfs vfs;
  statvfs("/", &vfs); // Assuming you want to check the root filesystem ("/")

  unsigned long disk_total = (vfs.f_blocks * vfs.f_frsize) / (1024 * 1024);
  unsigned long disk_free = (vfs.f_bfree * vfs.f_frsize) / (1024 * 1024);
  unsigned long disk_used = disk_total - disk_free;
  unsigned long disk_available = (vfs.f_bavail * vfs.f_frsize) / (1024 * 1024);

  char *json_data = (char *) malloc(1024 * sizeof(char));
  snprintf(json_data, 1024,
           "{"
           "\"hostname\": \"%s\","
           "\"cpu\": {\"model\": \"%s\", \"cores\": %d},"
           "\"memory\": {\"total\": %lu, \"used\": %lu, \"free\": %lu},"
           "\"disk\": {\"total\": %lu, \"used\": %lu, \"available\": %lu}"
           "}",
           uname_info.nodename, uname_info.machine, cpu_cores, total_memory, used_memory, free_memory,
           disk_total, disk_used, disk_available);

  return json_data;
}


int main() {
  // ... (same as before)

  char *json_data = get_system_info();

  // ... (same as before)

  printf("%s", json_data);
  free(json_data);

  return 0;
}
