//ALL IN ONE
//P4 - informs server to make an echo server
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    char buff[100];
    char ffifo[21] = "/tmp/ppfifo";
    mkfifo(ffifo, 0666);
    int p;

    //get process id of server
    printf("Reading server PID from pipe...\n");
    int fd1 = open(ffifo, O_RDONLY);
    read(fd1, buff, 100);
    close(fd1);
    if (buff[0] != '\0') {
        p = atoi(buff);
        buff[0] = '\0';
    }
    printf("Server pid => %d\n\n", p);

    //continuously take input from terminal
    while (1) {
        char str[100];
        scanf("%s", str);
        if (strcmp("signal", str) == 0) {
            printf("Signal sent to %d!\n", p);
            kill(p, SIGUSR1);
        }
    }

    return 0;
}
