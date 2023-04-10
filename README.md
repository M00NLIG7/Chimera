# Chimera Purple Team Malware

## Overview

Chimera is a purple team malware designed for security purposes. The name Chimera originates from Greek mythology, where the Chimera was a monstrous fire-breathing hybrid creature. The Chimera malware is fittingly named, as it is a hybrid tool capable of spreading across different operating systems, including Linux, Windows, and Cisco devices.

The primary goal of the Chimera malware is to aid security teams in identifying vulnerabilities and assessing the potential impact of a real-world attack on their infrastructure.

# Features

Identifies target operating systems (Linux, Windows, or Cisco)
Spreads across a given subnet using SSH or Windows Remote Management
Executes remote commands on infected hosts
Usage

```c
#include "net.h"

int main() {
    spread("192.168.1", "password");
}
```

The spread function takes a subnet and password as arguments and attempts to propagate the Chimera malware to all devices within the specified subnet. The program creates a thread for each IP address in the subnet and identifies the operating system type. Based on the detected OS, the malware will spread to the target using appropriate methods for the specific platform.

## Limitations

This program assumes that the devices within the specified subnet use the same password. In a real-world scenario, this may not always be the case.
The Chimera malware does not currently include any functionality for removing itself from infected devices.
It is essential to use this tool responsibly and only for security testing purposes on systems where you have the appropriate permissions.

## Disclaimer

Chimera is a proof of concept and is intended for educational purposes only. It should not be used for malicious intent or on systems for which you do not have authorization. The developer is not responsible for any misuse of this program.

<!-- zip -r archive.zip /path/to/archive -x file1.txt file2.txt mydir -->
<!-- tar -czvf archive.tar.gz --exclude=file1.txt --exclude=file2.txt --exclude=mydir /path/to/archive -->
<!-- net user Administrator NEW_PASSWORD -->
<!-- zip -r chimera_win.zip Chimera/ --exclude "./Chimera/bin/win*" --exclude "./Chimera/.git/*" -->
<!-- tar -czvf chimera_linux.tar.gz --exclude='bin/win*' --exclude=".git" Chimera/ -->
