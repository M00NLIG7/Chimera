#ifndef MAX_RESOURCES_H
#define MAX_RESOURCES_H

#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__)
    #include <unistd.h>
    #include <sys/resource.h>
#elif defined(_WIN32)
    #include <windows.h>
#endif


double calculate_max_resources(int cpu_cores, long memory_mb);
void print_max_resources();

#endif /* MAX_RESOURCES_H */
