//Iterative Connection Oriented Client server
//Client
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#define PORT 8080
#define	MAXLEN 1024

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

//MAIN
int main(int argc, char const *argv[])
{
	//when program is terminated, clean up socket
	signal(SIGINT, cleanup);

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
	else printf("Got hostname.\n");

	//setting up address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	//copying hostname hp->h_addr into address.sin_addr
	bcopy(hp->h_addr, (char *) &address.sin_addr, hp->h_length);	//returns NULL
	printf("Successfully finished byte copy of hostname.\n");

	//address we specify is the address of server we would like to comm. with
	if ((connect(sfd, (struct sockaddr*) &address, sizeof(address))) < 0)
		error("Error in connect()\n");

	//continuously send and receive
	char buff[MAXLEN];
	while (1) {
		//send message
		scanf("%s", buff);
		if (send(sfd, buff, strlen(buff), 0) < 0)
			error("Error in send()\n");

		//receive message
		while (1) {
			int n;
			char msg[25];
			if ((n = recv(sfd, &msg, sizeof(msg), 0)) < 0)
				error("Error in recv()\n");
			else if (n > 0) {
				printf("Recieved => %s\n", msg);
				break;
			}
		}
	}

	return 0;
}
