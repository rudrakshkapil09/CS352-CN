//NITW NEWS
//Reporter
//read messages from keyboard and put into famous fifo
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

//globals
char famous_fifo[20] = "/tmp/famous_fifo";

//MAIN
int main()
{
    //set up
    printf("\n---------------------------------------------------------\n");
    printf("REPORTER %d\nTo Stop Reporting, enter 'NOTHING'\n", getpid());
    printf("To Start Live Telecast Begin With A Decimal Number\n");
    printf("To Send Something To Document Writer Begin With '/d'\n");
    printf("---------------------------------------------------------\n");

    //make famous fifo
    mkfifo(famous_fifo, 0666);
    int editor_fd = open(famous_fifo, O_WRONLY | O_NONBLOCK);

    //continuosly report:
    char news[100] = "";
    while (1) {
        //get news
        printf("\nWhat's new? => ");
        fgets(news, 100, stdin);

        //nothing left - quit
        if (strcmp("NOTHING\n", news) == 0)
            break;

        //send to
        write(editor_fd, news, strlen(news)+1);
        printf("Reported to editor!");
    }

    printf("Going home...\n");
    system("exit");
}
