#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "process_manager.h"
#include "logger.h"

volatile sig_atomic_t child_exited = 0;
volatile sig_atomic_t alarm_triggered = 0;

int start_children(ChildProcess *children, int count)
{
    int i;
    pid_t pid;
    char msg[128];

    for (i = 0; i < count; i++) {
        snprintf(children[i].name, sizeof(children[i].name), "worker_%d", i + 1);
        children[i].restart_count = 0;
        children[i].is_running = 0;
        children[i].pid = -1;

        pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        } else if (pid == 0) {
            execl("./workers/worker", "worker", (char *)NULL);
            perror("execl");
            _exit(1);
        } else {
            children[i].pid = pid;
            children[i].is_running = 1;
            snprintf(msg, sizeof(msg), "Started %s with PID %d", children[i].name, pid);
            log_event(msg);
        }
    }

    return 0;
}
