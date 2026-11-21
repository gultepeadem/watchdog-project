#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "logger.h"

void log_event(const char *message)
{
    int fd;
    time_t now;
    struct tm *tm_info;
    char time_buf[32];
    char line[256];
    int len;

    now = time(NULL);
    tm_info = localtime(&now);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    len = snprintf(line, sizeof(line), "[%s] %s\n", time_buf, message);
    if (len < 0) {
        return;
    }

    fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open log file");
        return;
    }

    if (write(fd, line, (size_t)len) < 0) {
        perror("write log file");
    }

    close(fd);
}
