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
#define NUMS 2
#define	MAXLEN 1024

//error printing utility function
void error (char *str) {
	printf("%s", str);
	exit(-1);
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//global socket file descriptor
int sfd;
int ports[NUMS] = {8084,8083};	//different ports for different sockets

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

	//set up
    printf("\n---------------------------------------------------------\n");
    printf("CLIENT\n");
    printf("To Convert Input To LowerCase Enter 0\n");
    printf("To Convert Input To UpperCase Enter 1\n");
    printf("---------------------------------------------------------\n");

	//get port number
	int choice = -1;
	do {
		printf("Enter: ");
		scanf("%d", &choice);
	} while (choice < 0 || choice > 1);

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
	address.sin_port = htons(ports[choice]);

	//copying hostname hp->h_addr into address.sin_addr
	bcopy(hp->h_addr, (char *) &address.sin_addr, hp->h_length);	//returns NULL

	//address we specify is the address of server we would like to comm. with
	if ((connect(sfd, (struct sockaddr*) &address, sizeof(address))) < 0)
		error("Error in connect()\n");

	//send message
	while (1) {
		char buff[MAXLEN] = "";
		clear(buff);
		printf("Enter String To Convert => ");
		scanf("%s", buff);

		// clear message
		if (send(sfd, buff, strlen(buff), 0) < 0)
			error("Error in send()\n");

		clear(buff);
		//receive message
		int n;
		char msg[100];
		//while (1) {
			if ((n = recv(sfd, &msg, sizeof(msg), 0)) < 0)
				error("Error in recv()\n");
			else if (n > 0) {
				printf("Converted => %s\n", msg);
					//break;
				}
		//}
		clear(msg);
	}

	//end
	cleanup(SIGINT);
	printf("Thank you for using this program :)\n");
	return 0;
}
