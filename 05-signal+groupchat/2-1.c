//Group Chat Server - Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#define MAX_PROCS_PER_GROUP 10
#define GROUPS_COUNT 3

//globals
int process[GROUPS_COUNT][MAX_PROCS_PER_GROUP] = {{0}};  //to keep track of active clients
int currentGroupNumber;

//Writer functions
void Write(char buff[], int n)
{
    //get ID of sender
    int procID = 0, groupID = 0, i = 0, endOfString = 0;
    for (; buff[i] != 'X' && i < n; i++) //finds position of X
        endOfString++;

    //get number after X
    i++;
    for (; i < n; i++) {
        procID *= 10;
        procID += buff[i] - '0';
    }

    //check if valid message was sent
    if (procID >= MAX_PROCS_PER_GROUP) {
        printf("Incorrect Process ID at server!\n");
        return;
    }

    printf("\n=================================\n");
    printf("Received Message From %d\n", procID);
    printf("Sending to Group %d", currentGroupNumber);
    printf("\n-------------------------\n");
    process[currentGroupNumber][procID] = 1;

    //send the message to all other active procs
    for (int j = 1; j < MAX_PROCS_PER_GROUP; j++) {
        if ((process[currentGroupNumber][j] == 1) && j != procID) {
            printf("Sending Message To %d\n", j);

            //create addr to hold the name of the pipe by concat the number at the end
            char addr[20] = "./2-cpipe";  //eg 3-3-cpipe3 is pipe for client 3
            char int_string[3];
            sprintf(int_string, "%d", j);
            strcat(addr, int_string);

            //open the addr pipe and send the message, buff after removing X#
            int wfd = open(addr, O_WRONLY);
            write(wfd, buff, n);
        }
    }
    printf("-------------------------\n");
}

//MAIN
int main()
{

    //make famous fifos and open:
    mkfifo("./2-FF0", 0666);
    int ffo0_fd = open("./2-FF0", O_RDONLY | O_NONBLOCK | O_CREAT, 0644);
    mkfifo("./2-FF1", 0666);
    int ffo1_fd = open("./2-FF1", O_RDONLY | O_NONBLOCK | O_CREAT, 0644);
    mkfifo("./2-FF2", 0666);
    int ffo2_fd = open("./2-FF2", O_RDONLY | O_NONBLOCK | O_CREAT, 0644);


    //create a poll structure
    struct pollfd fds[GROUPS_COUNT];
    fds[0].fd = ffo0_fd;
    fds[1].fd = ffo1_fd;
    fds[2].fd = ffo2_fd;
    fds[0].events = POLLIN;
    fds[1].events = POLLIN;
    fds[2].events = POLLIN;

    //poll continuously
    char buff[100];
    while (1) {
      int r = poll(fds, 3, 2000); //%%
      if (r == 0)
        continue;

      printf("Checking For Inputs...\n");
      //some message arrived at server - handles multiple with loop
      for (int i = 0; i < GROUPS_COUNT; i++) { //%%
        if (fds[i].revents & POLLIN) {
          int n = read(fds[i].fd, buff, 100);
          currentGroupNumber = i;
          Write(buff, n);
        }
      }
    }
}
