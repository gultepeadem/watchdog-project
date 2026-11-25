#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <sys/types.h>
#include <signal.h>

#define MAX_CHILDREN 2
#define MAX_RESTARTS 3

typedef struct {
    pid_t pid;
    int restart_count;
    int is_running;
    char name[32];
} ChildProcess;

extern volatile sig_atomic_t child_exited;
extern volatile sig_atomic_t alarm_triggered;

int start_children(ChildProcess *children, int count);
int setup_child_signals(void);
void handle_child_exit(ChildProcess *children, int count);
void check_children_health(ChildProcess *children, int count);
void terminate_all_children(ChildProcess *children, int count);

#endif /* PROCESS_MANAGER_H */
