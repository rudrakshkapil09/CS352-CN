//ALL IN ONE
//P4 - informs server to make an echo server
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <signal.h>
using namespace std;

int main() {
    char buff[100];
    char ffifo[21] = "/tmp/ppfifo";
    int p;

    int fd1 = open(ffifo, O_RDONLY);
    read(fd1, buff, 100);
    close(fd1);

    //get process id
    if (buff[0] != '\0') {
        cout << buff << endl;
        p = atoi(buff);
        buff[0] = '\0';
    }

    //continuously take input from terminal
    while(1) {
        string str;
        cin >> str;
        cout << str << endl;
        if (str == "signal") {
            cout << "signal " << SIGUSR1 << "sent to " << p << endl;
            kill(p, SIGUSR1);
        }
    }
  return 0;
}
