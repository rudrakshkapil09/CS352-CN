//Chat Server - Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#define MAXPROCS 20

//globals
int process[MAXPROCS] = {0};  //to keep track of active clients

//Writer functions
void Write(char buff[], int n)
{
    //get ID of sender
    int procID = 0, i = 0;
    for (; buff[i] != 'X' && i < n; i++) //finds position of X
        ;

    //get number after X
    i++;
    for (; i < n; i++) {
        procID *= 10;
        procID += buff[i] - '0';
    }

    //check if valid message was sent
    if (procID >= MAXPROCS)
        return;
    printf("\n=================================\n");
    printf("Received Message From %d\n-------------------------\n", procID);
    process[procID] = 1;

    //send the message to all other active procs
    for (int j = 1; j < MAXPROCS; j++) {
        if ((process[j] == 1) && j != procID) {
            printf("Sending Message To %d\n", j);

            //create addr to hold the name of the pipe by concat the number at the end
            char addr[20] = "./3-3-cpipe";  //eg 3-3-cpipe3 is pipe for client 3
            char int_string[3];
            sprintf(int_string, "%d", j);
            strcat(addr, int_string);

            //open the addr pipe and send the message, buff
            int wfd = open(addr, O_WRONLY);
            write(wfd, buff, n);
        }
    }
    printf("-------------------------\n");
}

//MAIN
int main()
{
    //make famous fifo
    if (mkfifo("./3-3-FFO", 0666) == -1) {
        printf("FFO already exists\n");
    }

    int ffo_fd = open("./3-3-FFO", O_RDONLY);
    char buff[100];
    while (1) {
        int n = read(ffo_fd, buff, 100);
        Write(buff, n);
    }
}
