//ALL IN ONE
//P2 - COmmunicates with FIFO
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

//globals
char myfifo[21] = "/tmp/pfifo";
int fd1;

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//MAIN
int main() {
    //make fifo and get messages from terminal
	printf("Communicating via fifo...\n");
    //mkfifo(myfifo, 0666);
    while(1) {
		char buff[100];
        printf("Enter message: ");
		scanf("%s", buff);

        fd1 = open(myfifo, O_WRONLY);
        write(fd1, buff, strlen(buff)+1);
        printf("Writing Done!\n");

        close(fd1);
        clear(buff);
    }
  return 0;
}
