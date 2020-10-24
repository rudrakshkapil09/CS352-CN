//runner
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
    system("gcc F.c -o f");
    system("gcc S1.c -o s1");
    system("gcc S2.c -o s2");
    system("gcc C.c -o c");
}
