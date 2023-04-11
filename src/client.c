#include "client.h"

char *get_system_info() {
    char hostname[256];
#if defined(_WIN32)
    DWORD bufSize = sizeof(hostname);
    GetComputerNameA(hostname, &bufSize);
#else
    gethostname(hostname, sizeof(hostname));
#endif

    int cpu_cores;
    unsigned long long total_memory;
    unsigned long long used_memory;
    unsigned long long free_memory;
    unsigned long long disk_total;
    unsigned long long disk_free;
    unsigned long long disk_used;
    unsigned long long disk_available;

#if defined(_WIN32)
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    cpu_cores = sys_info.dwNumberOfProcessors;

    MEMORYSTATUSEX mem_info;
    mem_info.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&mem_info);
    total_memory = mem_info.ullTotalPhys / (1024 * 1024);
    used_memory = (mem_info.ullTotalPhys - mem_info.ullAvailPhys) / (1024 * 1024);
    free_memory = mem_info.ullAvailPhys / (1024 * 1024);

    ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
    GetDiskFreeSpaceExA(NULL, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);
    disk_total = lpTotalNumberOfBytes.QuadPart / (1024 * 1024);
    disk_used = (lpTotalNumberOfBytes.QuadPart - lpTotalNumberOfFreeBytes.QuadPart) / (1024 * 1024);
    disk_free = lpTotalNumberOfFreeBytes.QuadPart / (1024 * 1024);
    disk_available = lpFreeBytesAvailableToCaller.QuadPart / (1024 * 1024);
#else
    struct sysinfo sys_info;
    sysinfo(&sys_info);

    cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);

    total_memory = sys_info.totalram / (1024 * 1024);
    used_memory = (sys_info.totalram - sys_info.freeram) / (1024 * 1024);
    free_memory = sys_info.freeram / (1024 * 1024);

    struct statvfs vfs;
    statvfs("/", &vfs);
    disk_total = (vfs.f_blocks * vfs.f_frsize) / (1024 * 1024);
    disk_free = (vfs.f_bfree * vfs.f_frsize) / (1024 * 1024);
    disk_used = disk_total - disk_free;
    disk_available = (vfs.f_bavail * vfs.f_frsize) / (1024 * 1024);
#endif

    char *json_data = (char *)malloc(1024 * sizeof(char));
    snprintf(json_data, 1024,
             "{"
             "\"hostname\": \"%s\","
             "\"cpu\": {\"cores\": %d},"
             "\"memory\": {\"total\": %llu, \"used\": %llu, \"free\": %llu},"
             "\"disk\": {\"total\": %llu, \"used\": %llu, \"available\": %llu}"
             "}",
             hostname, cpu_cores, total_memory, used_memory, free_memory,
             disk_total, disk_used, disk_available);

    return json_data;
}

void evil_fetch() {
    char *json_data = get_system_info();
    printf("%s", json_data);
    free(json_data);
}
