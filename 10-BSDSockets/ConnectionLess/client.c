//Iterative Connectionless Client server
//Client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 8080
#define	MAXLEN 1024
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
	signal(SIGINT, cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error in socket()\n");
	else printf("Successfully set up socket.\n");

	//setting up address
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//get messages and send messages
	char buff[MAXSIZE];
	int n, len = sizeof(serverAddress);
	while (1) {
		//send message
		printf("Enter: ");
		scanf("%s\n", buff);
		sendto(sfd, buff, strlen(buff), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

		n = recvfrom(sfd, buff, MAXSIZE, 0, (struct sockaddr *) &serverAddress, (socklen_t*)&len);
		printf("Message from server => %s", buff);
		clear(buff);
	}
	return 0;
}
