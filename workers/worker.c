#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("Worker basladi. PID: %d\n", getpid());
    fflush(stdout);
    while (1) {
        sleep(1);
    }
    return 0;
}
