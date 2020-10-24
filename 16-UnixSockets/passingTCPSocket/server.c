//Iterative Connection Oriented Client server
//Server
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#define PORT 8080 //must be>1024
#define MAX_PENDING 10
#define	MAXSIZE 1024

//error printing utility function
void error (char *str) {
	perror(str);
	exit(-1);
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
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

//MAIN
int main(int argc, char const *argv[])
{
	printf("+--------------------------+\n");
	printf("|      	   SERVER          |\n");
	printf("+--------------------------+\n");

	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//setting up socket address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);	//returns 32 bit integer in host byte order
	address.sin_addr.s_addr = INADDR_ANY;

	//binding address to server
	if (bind(sfd, (struct sockaddr*) &address, sizeof(address)) < 0)
		error("Error in bind()\n");
	printf("Address successfully binded to socket.\n");

	//listening to client requests
	if ((listen(sfd, MAX_PENDING)) < 0)
		error("Error in listen()\n");
	printf("listen() successful.\n");

	//accept connection
	int len;
	int nsfd;
	if ((nsfd = accept(sfd, (struct sockaddr*) &address, (socklen_t *) &len)) < 0)
		perror("Error in accept()\n");
	printf("Connection accepted.\n");

	//get messages
	char buff[MAXSIZE];
	clear(buff);
	while(1) {
		while (1) {
			int n;
			if ((n = recv(nsfd, buff, sizeof(buff), 0)) < 0)
				error("Error in recv()");
			else if (n > 0) {
				printf("Recieved: %s\n",buff);
				break;
			}
		}
		//send message back to client
		strcat(buff, "-[server]\n\0");
		send(nsfd, &buff, strlen(buff), 0);
		clear(buff);
	}

	return 0;
}
