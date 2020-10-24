//ALL IN ONE
//Echo Server - created by S to communicate with client
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
const int x = 32;

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

int main(int argc,char *argv[])
{
    //get pid of server
    int server_pid = atoi(argv[0]);

    //send acknowledgement to client
    char start[100] = "\n-----------------------\nYou are now connected to echo server.\nSend quit to stop.\n";
    send(1, start, 100, 0);
    char pid[100];
    sprintf(pid, "PID => %d\n-----------------------\n", getpid());
    send(1, pid, 100, 0);

    //loop
    char buff[100];
    while (1) {
        clear(buff);
        recv(0, &buff, 100, 0);

        //check quit
        if (strcmp(buff, "quit") == 0) {
            kill(server_pid, SIGUSR2);
            exit(0);
        }

    	for (int i = 0; buff[i] != '\0'; i++)
    		buff[i] = buff[i] & ~x;
        send(1, buff, 100, 0);
    }

    return 0;
}
