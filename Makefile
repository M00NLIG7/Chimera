CC=gcc
CFLAGS=-I./include
OBJ=main.o src/net.o

EXE_LINUX=chimera
EXE_WINDOWS=chimera.exe

ifeq ($(OS),Windows_NT)
    CROSS_COMPILE=x86_64-w64-mingw32-
    RM=del /Q
else
    CROSS_COMPILE=
    RM=rm -f
endif

all: linux windows

linux: $(OBJ)
	$(CC) -o $(EXE_LINUX) $(OBJ) $(CFLAGS)

windows: $(OBJ)
	$(CROSS_COMPILE)$(CC) -o $(EXE_WINDOWS) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(EXE_LINUX) $(EXE_WINDOWS) $(OBJ)
