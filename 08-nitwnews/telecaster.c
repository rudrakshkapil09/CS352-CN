//NEWS
//Telecaster: takes in input and sends to newsreader via fifo
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

//globals
char telecast_fifo[20] = "/tmp/telecast_fifo";

//main
int main()
{
	//setup
	printf("\n---------------------------------------------------------\n");
    printf("LIVE TELECASTER\nBreaking News!\n");
    printf("---------------------------------------------------------\n");

	//open fifo
	mkfifo(telecast_fifo, 0666);
	int telecast_fd = open(telecast_fifo, O_WRONLY);

	//repeatedly scan and send
	char breaking_news[100];
	int breakflag = 0;
	while (!breakflag) {
		//scan
		printf("Enter: ");
		fgets(breaking_news, 100, stdin);

		//check for exit
		if (strcmp(breaking_news, "NOTHING\n") == 0)
			breakflag = 1;

		//send
		write(telecast_fd, breaking_news, strlen(breaking_news)+1);
	}

	//graceful exit
	close(telecast_fd);
	system("exit");
	return 0;
}
