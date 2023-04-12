#include "client.h"

double calculate_max_resources(int cpu_cores, long memory_mb) {
    double cpu_cores_weight = 4.0;
    double memory_weight = 2.0;
    double max_resources = (cpu_cores * cpu_cores_weight * 1024.0) + (memory_mb * memory_weight);
    return max_resources;
}

void evil_fetch() {
    int cpu_cores;
    long memory_mb;
    double max_resources;

    #if defined(__linux__)
        // Retrieve system resource information on Linux
        cpu_cores = sysconf(_SC_NPROCESSORS_CONF);
        memory_mb = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / 1024 / 1024; // Convert from bytes to megabytes
    #elif defined(_WIN32)
        // Retrieve system resource information on Windows
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        cpu_cores = sys_info.dwNumberOfProcessors;
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(mem_info);
        GlobalMemoryStatusEx(&mem_info);
        memory_mb = mem_info.ullTotalPhys / 1024 / 1024; // Convert from bytes to megabytes
    #endif

    // Calculate maximum resources
    max_resources = calculate_max_resources(cpu_cores, memory_mb);

    printf("Max resources: %f MB\n", max_resources);
}
