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
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_term;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);

    printf("Worker started. PID: %d\n", getpid());
    fflush(stdout);

    while (running) {
        sleep(1);
    }

    printf("Worker exiting normally. PID: %d\n", getpid());
    fflush(stdout);
    return 0;
}
