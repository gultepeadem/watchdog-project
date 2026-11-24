#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "process_manager.h"
#include "logger.h"

volatile sig_atomic_t child_exited = 0;
volatile sig_atomic_t alarm_triggered = 0;

static void handle_sigchld(int signum)
{
    (void)signum;
    child_exited = 1;
}

static void handle_sigalrm(int signum)
{
    (void)signum;
    alarm_triggered = 1;
}

int setup_child_signals(void)
{
    struct sigaction sa_chld;
    struct sigaction sa_alrm;

    memset(&sa_chld, 0, sizeof(sa_chld));
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        perror("sigaction SIGCHLD");
        return -1;
    }

    memset(&sa_alrm, 0, sizeof(sa_alrm));
    sa_alrm.sa_handler = handle_sigalrm;
    sigemptyset(&sa_alrm.sa_mask);
    sa_alrm.sa_flags = SA_RESTART;

    if (sigaction(SIGALRM, &sa_alrm, NULL) < 0) {
        perror("sigaction SIGALRM");
        return -1;
    }

    alarm(5);
    return 0;
}

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

void handle_child_exit(ChildProcess *children, int count)
{
    int status;
    pid_t pid;
    int i;
    char msg[128];

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (i = 0; i < count; i++) {
            if (children[i].pid != pid) {
                continue;
            }

            children[i].is_running = 0;

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                snprintf(msg, sizeof(msg), "%s (PID %d) exited normally", children[i].name, pid);
                log_event(msg);
            } else {
                if (WIFSIGNALED(status)) {
                    snprintf(msg, sizeof(msg), "%s (PID %d) terminated by signal %d",
                             children[i].name, pid, WTERMSIG(status));
                } else {
                    snprintf(msg, sizeof(msg), "%s (PID %d) exited with error code %d",
                             children[i].name, pid, WEXITSTATUS(status));
                }
                log_event(msg);

                if (children[i].restart_count < MAX_RESTARTS) {
                    pid_t new_pid = fork();
                    if (new_pid < 0) {
                        perror("fork restart");
                    } else if (new_pid == 0) {
                        execl("./workers/worker", "worker", (char *)NULL);
                        perror("execl restart");
                        _exit(1);
                    } else {
                        children[i].pid = new_pid;
                        children[i].is_running = 1;
                        children[i].restart_count++;
                        snprintf(msg, sizeof(msg), "Restarted %s with new PID %d (attempt %d/%d)",
                                 children[i].name, new_pid, children[i].restart_count, MAX_RESTARTS);
                        log_event(msg);
                    }
                } else {
                    snprintf(msg, sizeof(msg), "%s reached max restart limit (%d), not restarting",
                             children[i].name, MAX_RESTARTS);
                    log_event(msg);
                }
            }
            break;
        }
    }
}

void terminate_all_children(ChildProcess *children, int count)
{
    int i;
    int elapsed;
    int remaining;
    int status;
    pid_t res;
    char msg[128];

    for (i = 0; i < count; i++) {
        if (children[i].is_running && children[i].pid > 0) {
            snprintf(msg, sizeof(msg), "Sending SIGTERM to %s (PID %d)", children[i].name, children[i].pid);
            log_event(msg);
            if (kill(children[i].pid, SIGTERM) < 0) { perror("kill SIGTERM"); }
        }
    }

    for (elapsed = 0; elapsed < 5; elapsed++) {
        remaining = 0;
        for (i = 0; i < count; i++) {
            if (children[i].is_running && children[i].pid > 0) {
                res = waitpid(children[i].pid, &status, WNOHANG);
                if (res > 0) {
                    children[i].is_running = 0;
                    snprintf(msg, sizeof(msg), "%s (PID %d) terminated gracefully", children[i].name, children[i].pid);
                    log_event(msg);
                } else if (res == 0) {
                    remaining++;
                } else {
                    children[i].is_running = 0;
                }
            }
        }
        if (remaining == 0) {
            break;
        }
        sleep(1);
    }

    for (i = 0; i < count; i++) {
        if (children[i].is_running && children[i].pid > 0) {
            snprintf(msg, sizeof(msg), "Timeout reached, sending SIGKILL to %s (PID %d)", children[i].name, children[i].pid);
            log_event(msg);
            if (kill(children[i].pid, SIGKILL) < 0) { perror("kill SIGKILL"); }
            waitpid(children[i].pid, &status, 0);
            children[i].is_running = 0;
        }
    }
}
