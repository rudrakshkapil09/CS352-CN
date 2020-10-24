//ALL IN ONE
//P3 - communicates with popen
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/socket.h>

int main() {
    while (1) {
        sleep(10);
        char buff[100] = "Message From P3";
        write(1, buff, strlen(buff)+1);
        fflush(stdout);
    }
    return 0;
}
