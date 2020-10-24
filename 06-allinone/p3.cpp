//ALL IN ONE
//P3 - communicates with popen
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main() {
    char buff[100] = "Message From P3";
    while (1) {
        sleep(10);
        write(1, buff, strlen(buff)+1);
    }
    return 0;
}
