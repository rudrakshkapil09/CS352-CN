//Concurrent Connection Oriented Client server
//Server
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#define PORT 8080//must be>1024
#define MAX_PENDING 10
#define	MAXSIZE 1024

//error printing utility function
void error (char *str) {
	perror(str);
	exit(-1);
}

//global socket file descriptor - original 'table'
int sfd;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(sfd, SHUT_RDWR);	//disallow further reads & writes
	close(sfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//service function to handle clients
void service_fn(int nsfd)
{
	char buff[MAXSIZE];
	clear(buff);
	int n;
	printf("nsfd: %d\n", nsfd);
	while (1) {
		//get message
		if ((n = recv(nsfd, buff, sizeof(buff), 0)) < 0)
			error("Error in recv()\n");

		else if (n > 0) {
			printf("Recieved: %s\n", buff);
			//strcat(buff, " - from server\n");
			printf("To Send => %s\n\n", buff);
			send(nsfd, buff, strlen(buff), 0);
			clear(buff);
		}
	}
}

//MAIN
int main(int argc, char const *argv[])
{
	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//set socket options
	int true = 1;
	int rtn = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
	assert(rtn == 0);

	//setting up socket address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);	//returns 32 bit integer in host byte order
	address.sin_addr.s_addr = INADDR_ANY;

	//binding address to server
	if (bind(sfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		if (shutdown(sfd, SHUT_RDWR) < 0)
			error("Error in shutdown()\n");
		close(sfd);
		error("Error in bind()\n");
	}
	printf("Address successfully binded to socket\n");

	//listening to client requests
	if ((listen(sfd, MAX_PENDING)) < 0)
		error("Error in listen()\n");
	printf("listen() successful.\n");

	int len;
	int nsfd;
	char buff[MAXSIZE];
	while (1) {
		//accept connection request
		if ((nsfd = accept(sfd, (struct sockaddr*) &address, &len)) < 0)
			perror("Error in accept()\n");
		printf("Connection accepted.\n");

		//fork to handle more clients
		int c = fork();
		if (c < 0)
			error("Error in fork()");

		//parent - go accept more clients
		else if (c > 0)
			close(nsfd);

		//child - handle client
		else if (c == 0) {
			close(sfd);
			service_fn(nsfd);
		}
	}
	return 0;
}
