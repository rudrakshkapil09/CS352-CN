//Polling two file descriptors: stdin and a pipe
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

int main() {

	char buffer[100];
	int p1[2];
	pipe(p1);

	int stdin_fd = dup(0);

	struct pollfd fds[2];
	fds[0].fd = p1[0];			//0 -> pipe
	fds[1].fd = stdin_fd;		//1 -> keyboard

	fds[0].events = POLLIN;
	fds[1].events = POLLIN;


	int c = fork();

	//parent
	if (c > 0) {
		while (1) {
			int r = poll(fds,2,2000);
			if (r == 0)								//no message
				continue;
			else if (fds[0].revents & POLLIN) {			//message is there in pipe
				printf("From Pipe: ");
				read(fds[0].fd, buffer, sizeof(buffer));
				printf("%s", buffer);
				fflush(stdout);
				clear(buffer);
			}
			else if (fds[1].revents & POLLIN) {			//keyboard message
				printf("From Keyboard: ");
				read(fds[1].fd, buffer, sizeof(buffer));
				printf("%s\n", buffer);
				fflush(stdout);
				clear(buffer);
			}
		}
	}
	//child
	else {
		while (1) {
			int r = rand() % 10;
			sleep(r);
			close(p1[0]);
			char * string = "Input from the pipe\n";
			write(p1[1], string, strlen(string));
		}
	}



}
