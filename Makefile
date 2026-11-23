CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
OBJS = src/main.o src/signal_manager.o src/process_manager.o src/logger.o
EXE = watchdog
WORKER = workers/worker

all: $(EXE) $(WORKER)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(WORKER): workers/worker.c
	$(CC) $(CFLAGS) -o $@ $<

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE) $(WORKER) watchdog.log
