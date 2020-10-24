//ALL IN ONE runner
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
    int p;
    if ((p = system("gcc server.c -o server")) < 0)
        printf("System perror Server\n");

    if ((p = system("gcc p2.c -o p2")) < 0)
        printf("System error P2\n");

    if ((p = system("gcc p3.c -o p3")) < 0)
        printf("System error P3\n");

    if ((p = system("gcc p4.c -o p4")) < 0)
        printf("System error P4\n");

    if ((p = system("gcc p5.c -o p5")) < 0)
        printf("System error P5\n");

    if ((p = system("gcc echo.c -o echo")) < 0)
        printf("System fault Echo\n");

	if ((p = system("gcc client.c -o client")) < 0)
		printf("System fault Client\n");
}
