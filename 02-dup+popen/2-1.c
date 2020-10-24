//Make first three couts go to file, and next three go to std output
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	 int fd = open("output2-1.txt", O_WRONLY);

	 int nfd = dup(1);
	 dup2(fd, 1);
	 printf("1");
	 printf("2");
	 printf("3");

	 fflush(stdout);
	 close(fd);

	 dup2(nfd, 1);
	 close(nfd);
	 printf("1");
	 printf("2");
	 printf("3");
}
