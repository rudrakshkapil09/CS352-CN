//compiles all
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

const int x = 32;


//MAIN
int main()
{
	system("gcc server.c -o server");
	system("gcc client.c -o client");
	system("gcc s1.c -o s1");
	system("gcc s2.c -o s2");
}
