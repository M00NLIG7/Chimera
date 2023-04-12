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


// Define a struct that will hold the get_system_info() data
typedef struct {
  char hostname[256];
  char cpu_model[256];
  int cpu_cores;
  unsigned long total_memory;
  unsigned long used_memory;
  unsigned long free_memory;
  unsigned long disk_total;
  unsigned long disk_used;
  unsigned long disk_available;
} system_info_t;

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

int main() {
    system_info_t system_info;

    char* sys_data = get_system_info();
    // Create a system_info_t object using the data from get_system_info()
    sscanf(sys_data, "{\"hostname\": \" %31[^\"] \",\"cpu\": {\"model\": \" %31[^\"] \", \"cores\": %d },\"memory\": {\"total\": %d , \"used\": %d , \"free\": %d },\"disk\": {\"total\": %d , \"used\": %d , \"available\": %d }}",
        system_info.hostname, system_info.cpu_model, &system_info.cpu_cores, &system_info.total_memory, &system_info.used_memory, &system_info.free_memory, &system_info.disk_total, &system_info.disk_used, &system_info.disk_available);

    // buffer = get_system_info();
    // sscanf(buffer, "{\"hostname\": \" %31[^\"] \",\"cpu\": {\"model\": \" %31[^\"] \", \"cores\": %d },\"memory\": {\"total\": %d , \"used\": %d , \"free\": %d },\"disk\": {\"total\": %d , \"used\": %d , \"available\": %d }}",
        // hostname, cpu_model, &cpu_cores, &memory_total, &memory_used, &memory_free, &disk_total, &disk_used, &disk_available);
    // print all the attributes of system_info 
    printf("Hostname: %s, CPU Model: %s, CPU Cores: %d, Total Memory: %lu, Used Memory: %lu, Free Memory: %lu, Disk Total: %lu, Disk Used: %lu, Disk Available: %lu", system_info.hostname, system_info.cpu_model, system_info.cpu_cores, system_info.total_memory, system_info.used_memory, system_info.free_memory, system_info.disk_total, system_info.disk_used, system_info.disk_available);
    
    free(sys_data);
    return 0;
}