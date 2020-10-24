//Using popen to make std ip and op of two processes equal to each others op and ip
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
		int stdout_fd = dup(1);
		int stdin_fd = dup(0);

		FILE *fin1, *fin2;
		fin1 = popen("./2-2-2", "r");
		fin2 = popen("./2-2-2", "w");

		printf("Starting:\n");

		int out_fd = fileno(fin2);
		int in_fd = fileno(fin1);

		char message[1000] = "P1 Sending This Message To P2.\n";
		printf("Message => %s", message);
		write(out_fd, message, strlen(message)+1);
		//printf("P1 Sending This Message To P2.\n");	//this will be sent to stdin of P2
		//fflush(stdout);
		//scanf("%s", message);
		read(in_fd, message, 1000);

		//dup2(stdout_fd, 1);
		//dup2(stdin_fd, 0);
		printf("%s\n", message);

		close(stdout_fd);
		close(stdin_fd);
		close(in_fd);
		close(out_fd);
		pclose(fin1);
		pclose(fin2);
		return 0;
}
