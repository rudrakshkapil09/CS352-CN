//ALL IN ONE
//P5 - output of server
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>

int main() {
    char buff[100];
    while(1) {
        scanf("%s", buff);
        printf("%s\n", buff);
    }
    return 0;
}
