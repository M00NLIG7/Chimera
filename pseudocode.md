I need a distributed node system which can connect to both windows and linux then replicate itself until every machine on the network has a node on it. We can assume winrm and ssh will both be closed on windows. How would i connect using something like psexec if I am running the starting node on linux

project/
├── src/
│ ├── main.c
│ ├── spread.c
│ ├── spread.h
│ └── ...
├── win64/
│ ├── psexec.exe
│ └── winexe.exe
├── win32/
│ ├── psexec.exe
│ └── winexe.exe
├── linux64/
│ └── winexe
└── linux32/
└── winexe
