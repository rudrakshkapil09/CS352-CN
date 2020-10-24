//Iterative Connectionles Client server
//Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//setting up socket address
	struct sockaddr_in serverAddress, clientAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	memset(&clientAddress, 0, sizeof(clientAddress));

	//server information
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);	//returns 32 bit integer in host byte order
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//binding address to socket
	if (bind(sfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		error("Error in bind()\n");
	printf("Address successfully binded to socket.\n");

	//get messages and send messages
	char buff[MAXSIZE];
	char reply[100] = "Reply from server!\n";
	int n, len = sizeof(clientAddress);
	while (1) {
		//get message
		clear(buff);
		n = recvfrom(sfd, buff, MAXSIZE, 0, (struct sockaddr *) &clientAddress, (socklen_t*) &len);
		printf("Message from client => %s", buff);
		clear(buff);

		//send message
		sendto(sfd, reply, strlen(reply), 0, (struct sockaddr *) &clientAddress, len);
		printf("\nReply sent!\n");
	}
	return 0;
}
