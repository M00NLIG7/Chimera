CC=gcc
CC_WIN=x86_64-w64-mingw32-gcc
CFLAGS=-I./include
OBJ=main.o src/net.o
OBJ_WIN=main_win.o src/net_win.o

EXE=chimera
EXE_WIN=chimera.exe

ifeq ($(OS),Windows_NT)
    RM=del /Q
else
    RM=rm -f
endif

all: linux windows

linux: $(EXE)

windows: $(EXE_WIN)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -static -static-libgcc -static-libstdc++


$(EXE_WIN): $(OBJ_WIN)
	$(CC_WIN) -o $@ $^ $(CFLAGS) -lws2_32 -pthread

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%_win.o: %.c
	$(CC_WIN) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(EXE) $(EXE_WIN) $(OBJ) $(OBJ_WIN)
