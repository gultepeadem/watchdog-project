#include <stdio.h>
#include <string.h>
#include "signal_manager.h"

volatile sig_atomic_t shutdown_requested = 0;
volatile sig_atomic_t status_requested = 0;

static void handle_shutdown_signal(int signum)
{
    (void)signum;
    shutdown_requested = 1;
}

static void handle_usr1_signal(int signum)
{
    (void)signum;
    status_requested = 1;
}

int setup_signal_handlers(void)
{
    struct sigaction sa_shutdown;
    struct sigaction sa_usr1;

    memset(&sa_shutdown, 0, sizeof(sa_shutdown));
    sa_shutdown.sa_handler = handle_shutdown_signal;
    sigemptyset(&sa_shutdown.sa_mask);
    sa_shutdown.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa_shutdown, NULL) < 0) {
        perror("sigaction SIGINT");
        return -1;
    }

    if (sigaction(SIGTERM, &sa_shutdown, NULL) < 0) {
        perror("sigaction SIGTERM");
        return -1;
    }

    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = handle_usr1_signal;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        perror("sigaction SIGUSR1");
        return -1;
    }

    return 0;
}
