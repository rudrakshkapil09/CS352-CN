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
    if ((p = system("gcc server.c -o server")) < 0)
        printf("System perror Server\n");

    if ((p = system("gcc group1.c -o group1")) < 0)
        printf("System error G1\n");

    if ((p = system("gcc group2.c -o group2")) < 0)
        printf("System error G2\n");
}
