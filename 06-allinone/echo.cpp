//ALL IN ONE
//Echo Server - created by S to communicate with client
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

int main(int argc,char *argv[])
{
    
    //create fifo with client process name
    char ffifo[21] = "/tmp/1";
    strcat(ffifo, argv[1]);
    mkfifo(ffifo, 0666);

    //get message
    char buff[100];
    cout << "Enter message: ";
    cin >> buff;

    //send signal to client that it is connected to echo server now
    int p = atoi(argv[1]);
    kill(p, SIGUSR1);

    //echo to the fifo until exit is typed
    while (strcmp(buff, "exit") != 0) {
        cout << "Enter message: ";
        cin >> buff;
        if (strcmp(buff, "exit") == 0)
            kill(p, SIGUSR1);

        int fd1 = open(ffifo, O_WRONLY);
        write(fd1, buff, strlen(buff)+1);
        close(fd1);
    }

    return 0;
}
