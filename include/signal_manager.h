#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include <signal.h>

extern volatile sig_atomic_t shutdown_requested;
extern volatile sig_atomic_t status_requested;

int setup_signal_handlers(void);

#endif /* SIGNAL_MANAGER_H */
