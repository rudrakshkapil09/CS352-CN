//Broadcast Server - Broadcast
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>

//MAIN
int main()
{
		printf("\n---------[In Broadcast Server Now]---------");
		char m[100];
		scanf("%s", m);
		printf("\nMessage Received : %s", m);

		FILE *fp1 = popen("./3-2-3", "w");
		FILE *fp2 = popen("./3-2-4", "w");
		FILE *fp3 = popen("./3-2-5", "w");
		int fd1 = fileno(fp1);
		int fd2 = fileno(fp2);
		int fd3 = fileno(fp3);

		write(fd1, m, 100);
		write(fd2, m, 100);
		write(fd3, m, 100);
		return 0;
}
