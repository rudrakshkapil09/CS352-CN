#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>

int main(){
    char in[100];
    int fd = open("1.txt",O_CREAT|O_WRONLY);
    dup2(fd,1);

        bzero(in,100);
        scanf("%s",in);
        printf("%s\n",in);
        bzero(in,100);
        scanf("%s",in);
        printf("%s\n",in);
        bzero(in,100);
        scanf("%s",in);
        printf("%s\n",in);

    return 0;
}
