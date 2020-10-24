//Proxy runner
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

    if ((p = system("gcc s1.c -o s1")) < 0)
        printf("System error s1\n");

    if ((p = system("gcc s2.c -o s2")) < 0)
        printf("System error s2\n");

    if ((p = system("gcc s3.c -o s3")) < 0)
        printf("System error s3\n");

	if ((p = system("gcc client.c -o client")) < 0)
		printf("System fault Client\n");
}
