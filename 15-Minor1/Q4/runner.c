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
    system("gcc ess.c -o ess");
    system("gcc c1.c -o c1");
    system("gcc c.c -o c");
}
