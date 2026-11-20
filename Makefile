CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
EXE = watchdog

all: $(EXE)

$(EXE): src/main.c
	$(CC) $(CFLAGS) -o $(EXE) src/main.c

clean:
	rm -f $(EXE) *.o src/*.o workers/worker
