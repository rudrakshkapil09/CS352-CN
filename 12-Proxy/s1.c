//Service 1 - Converts to Lowercase
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <netdb.h>
#define PORT 8091
const int x = 32;

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//error printing utility function
void error (char *str) {
	perror(str);
	exit(-1);
}

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
int main()
{
	//set up
	printf("\n---------------------------------------------------------\n");
	printf("SERVER 1\n");
	printf("Converts to LowerCase\n");
	printf("---------------------------------------------------------\n");

	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	else printf("Successfully set up socket.\n");

	//get host name - because ipv4 is specified, function copies name into the h_name field
	//and its struct in_addr equivalent into the h_addr_list[0] field of the returned hostent struct
	struct hostent *hp;
	hp = gethostbyname("127.0.0.1");
	if (hp == NULL)
		error("Error in gethostname()\n");

	//setting up address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	//copying hostname hp->h_addr into address.sin_addr
	bcopy(hp->h_addr, (char *) &address.sin_addr, hp->h_length);	//returns NULL

	//address we specify is the address of server we would like to comm. with
	if ((connect(sfd, (struct sockaddr*) &address, sizeof(address))) < 0)
		error("Error in connect()\n");


	char buff[100];
	while (1) {
		clear(buff);
		recv(sfd, &buff, sizeof(buff), 0);
		//read(0, buff, 100);
		printf("Received => %s\n", buff);

		for (int i = 0; buff[i] != '\0'; i++)
			buff[i] = buff[i] | x;

		printf("Sending => %s\n\n", buff);
		send(sfd, buff, strlen(buff), 0);
		//write(1, buff, strlen(buff)+1);
	}
 	return 0;
}
