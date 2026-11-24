#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void handle_term(int signum)
{
    (void)signum;
    running = 0;
}

int main(void)
{
    struct sigaction sa_term;
    struct sigaction sa_ign;

    memset(&sa_term, 0, sizeof(sa_term));
    sa_term.sa_handler = handle_term;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) < 0) {
        perror("worker sigaction SIGTERM");
        return 1;
    }

    memset(&sa_ign, 0, sizeof(sa_ign));
    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_flags = 0;
    if (sigaction(SIGINT, &sa_ign, NULL) < 0) {
        perror("worker sigaction SIGINT");
        return 1;
    }

    printf("Worker started. PID: %d\n", getpid());
    fflush(stdout);

    while (running) {
        sleep(1);
    }

    printf("Worker exiting normally. PID: %d\n", getpid());
    fflush(stdout);
    return 0;
}
