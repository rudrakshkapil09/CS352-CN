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
    system("gcc server.c -o server");
    system("gcc s1.c -o s1");
    system("gcc s2.c -o s2");
    system("gcc s3.c -o s3");
    system("gcc client.c -o client");
}
