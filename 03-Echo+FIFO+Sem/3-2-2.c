//Broadcast Server - Sender
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>

//MAIN
int main()
{
    char mess[100];
    printf("\n---------[In Sender Now]---------");
    printf("\nEnter the Message to be sent to Broadcast Server:\n");
    scanf("%s", mess);
    FILE *fp = popen("./3-2-1","w");
    int fd = fileno(fp);
    write(fd, mess, strlen(mess));
    return 0;
}
