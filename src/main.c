#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "signal_manager.h"
#include "process_manager.h"
#include "logger.h"

int main(void)
{
    ChildProcess children[MAX_CHILDREN];
    sigset_t block_mask;
    sigset_t old_mask;
    char msg[128];

    snprintf(msg, sizeof(msg), "Watchdog started with PID %d", getpid());
    log_event(msg);
    printf("%s\n", msg);

    if (setup_signal_handlers() < 0) {
        fprintf(stderr, "Failed to setup signal handlers\n");
        return 1;
    }

    if (setup_child_signals() < 0) {
        fprintf(stderr, "Failed to setup child signal handlers\n");
        return 1;
    }

    if (start_children(children, MAX_CHILDREN) < 0) {
        fprintf(stderr, "Failed to start child processes\n");
        return 1;
    }

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGTERM);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGALRM);

    if (sigprocmask(SIG_BLOCK, &block_mask, &old_mask) < 0) {
        perror("sigprocmask");
        terminate_all_children(children, MAX_CHILDREN);
        return 1;
    }

    while (!shutdown_requested) {
        if (!child_exited && !status_requested && !alarm_triggered) {
            sigsuspend(&old_mask);
        }

        if (child_exited) {
            child_exited = 0;
            handle_child_exit(children, MAX_CHILDREN);
        }

        if (status_requested) {
            status_requested = 0;
            log_status_report(children, MAX_CHILDREN);
        }

        if (alarm_triggered) {
            alarm_triggered = 0;
            check_children_health(children, MAX_CHILDREN);
            alarm(5);
        }
    }

    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    alarm(0);
    log_event("Shutdown signal received, initiating graceful shutdown");
    terminate_all_children(children, MAX_CHILDREN);
    log_event("Watchdog shutdown complete");
    printf("Watchdog shutdown complete.\n");

    return 0;
}
