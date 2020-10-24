//Bypass runner
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
    int p;
    if ((p = system("gcc station.c -o station")) < 0)
        printf("System perror Server\n");

    if ((p = system("gcc train.c -o train")) < 0)
        printf("System error G1\n");

    if ((p = system("gcc platform.c -o platform")) < 0)
        printf("System error G2\n");
}
