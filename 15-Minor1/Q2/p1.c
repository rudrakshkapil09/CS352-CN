#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

int main(){
    system("gcc p2.c -o p2");
    system("gcc p3.c -o p3");
    system("gcc p4.c -o p4");
    system("gcc p5.c -o p5");

    int fd2 = fileno(popen("./p2","r"));
    int fd3 = fileno(popen("./p3","r"));
    int fd4 = fileno(popen("./p4","r"));
    int fd5 = fileno(popen("./p5","w"));

    printf("Going to dup to fd5 now\n");

    char in[100];
    struct pollfd fds[3];
    fds[0].fd=fd2;
    fds[1].fd=fd3;
    fds[2].fd=fd4;
    fds[0].events=POLLIN;
    fds[1].events=POLLIN;
    fds[2].events=POLLIN;
    //dup2(fd5,1);
    printf("Check\n");
    while(1){
        int ret = poll(fds,3,0);
        if(ret<1)
            continue;
        else{
            if((fds[0].revents&POLLIN)==1){
                dup2(fds[0].fd,0);
                bzero(in,100);
                scanf("%s",in);
                printf("%s",in);
            }
            else if((fds[1].revents&POLLIN)==1){
                dup2(fds[1].fd,0);
                bzero(in,100);
                scanf("%s",in);
                printf("%s",in);
            }
            else{
                dup2(fds[2].fd,0);
                bzero(in,100);
                scanf("%s",in);
                printf("%s",in);
            }
        }
    }
}
