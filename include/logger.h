#ifndef LOGGER_H
#define LOGGER_H

#include "process_manager.h"

#define LOG_FILE "watchdog.log"

void log_event(const char *message);
void log_status_report(const ChildProcess *children, int count);

#endif /* LOGGER_H */
