//Group Chat Server - Client 3
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

//globals
int groupChoice;
int clientID_int = 3;
char clientID[3] = "X3";
char pipeName[20] = "./2-cpipe3";
char FFOs[3][20] = {"./2-FF0", "./2-FF1", "./2-FF2"};
pthread_t rdth, wrth;

//read thread - read from
void *rf(void *args)
{
    while (1) {
        //open client pipe and read message
        char buff[100];
        int rfd = open(pipeName, O_RDONLY);
        int n = read(rfd, buff, 100);

        //Get ID of sender
        int procID = 0, i = 0;
        for (; buff[i] != 'X' && i < n; i++) //finds position of X
            ;
        buff[i] = '\0';   //so that the ID is not printed after the message

        //get number after X
        i++;
        for (; i < n; i++) {
            procID *= 10;
            procID += buff[i] - '0';
            buff[i] = '\0';
        }

        //print message
        printf("C-%2d\t=> %s\n", procID, buff);
    }
}

//write thread
void *wf(void *args)
{
    while (1) {
        //get message from keyboard and concat id at end
        char buff[100];
        scanf("%s", buff);
        int n = strlen(buff);
        strcat(buff, clientID);

        //send message via famous fifo to server
        int wfd = open(FFOs[groupChoice], O_WRONLY);
        write(wfd, buff, strlen(buff));

        int i = 0;
        for (; buff[i] != 'X'; i++);
        for (; buff[i] != '\0'; i++)
            buff[i] = '\0';
        printf("C-%2d\t=> %s\n", clientID_int, buff);
    }
}

//MAIN
int main()
{
    mkfifo(pipeName, 0666);

    while (1) {
      printf("Which group would you like to join (0-2)? ");
      scanf("%d", &groupChoice);
      if (groupChoice >= 0 && groupChoice < 3)
        break;
    }
    printf("\n\n=================\nWelcome to Group %d\n=================\n", groupChoice);

    //create threads
    pthread_create(&rdth, NULL, rf, NULL);
    pthread_create(&wrth, NULL, wf, NULL);

    //wait for them to finish - never happens
    pthread_join(rdth, NULL);
    pthread_join(wrth, NULL);
    return 0;
}
