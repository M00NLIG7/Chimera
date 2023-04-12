#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

double calculate_max_resources(int cpu_cores, long memory_gb) {
    double cpu_cores_weight = 1.0;
    double memory_weight = 1.5;
    double max_resources = (cpu_cores * cpu_cores_weight) + (memory_gb * memory_weight);
    return max_resources;
}

int main() {
    int cpu_cores;
    long memory_gb;
    double max_resources;

    // Retrieve system resource information
    cpu_cores = sysconf(_SC_NPROCESSORS_CONF);
    memory_gb = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / 1024 / 1024 / 1024; // Convert from bytes to GB

    // Calculate maximum resources
    max_resources = calculate_max_resources(cpu_cores, memory_gb);

    printf("Max resources: %f\n", max_resources);

    return 0;
}
