//ALL IN ONE
//P2 - COmmunicates with FIFO
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
using namespace std;

//globals
char myfifo[21] = "/tmp/pfifo";
char buff[100];
int fd1;

//MAIN
int main() {
    //make fifo and get messages from terminal
    mkfifo(myfifo, 0666);
    while(1) {
        cout << "Enter message: ";
        cin >> buff;
        fd1 = open(myfifo, O_WRONLY);

        write(fd1, buff, strlen(buff)+1);
        cout << "Writing Done!\n";
        close(fd1);
    }
  return 0;
}
