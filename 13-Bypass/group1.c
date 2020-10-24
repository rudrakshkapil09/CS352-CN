//Bypass server
//Client 2 from group 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#define GROUP1_PORT 57579
#define SERVER_PORT 12211
#define MAX_PENDING 10

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
int csfd;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(csfd, SHUT_RDWR);	//disallow further reads & writes
	close(csfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//used to set address of process
void setAddress(struct sockaddr_in *address, int portNum)
{
    (*address).sin_family = AF_INET;
    (*address).sin_port = htons(portNum);
    (*address).sin_addr.s_addr = INADDR_ANY;
}

int main()
{
	printf("\n+------------------+");
    printf("\n|  Group 1 Client  |");
    printf("\n+------------------+\n");

    //when program is terminated, clean up sockets
	signal(SIGINT,cleanup);

    //open socket
	if ((csfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error in socket() 1\n");
	printf("Successfully set up socket.\n");

    //set addresses for communication
    struct sockaddr_in my_address, server_address, g2_address;
    socklen_t server_len, g2_len;
    setAddress(&my_address, GROUP1_PORT);
    setAddress(&server_address, SERVER_PORT);

	int true = 1;
	setsockopt(csfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int));


    //binding address to socket
	/*
	if (bind(csfd, (struct sockaddr *) &my_address, sizeof(my_address)) < 0) {
		if (shutdown(csfd, SHUT_RDWR) < 0)
			printf("Error in shutdown() 1\n");
		//close(csfd);
		printf("Error in bind() 1n");
	}
	printf("Address successfully binded to socket.\n");
	*/

    //get number of group2 client
    printf("Enter group 2 number you wish to communicate with: ");
    char choice[100];
    scanf("%s", choice);

	//get local address
    //getsockname(csfd, (struct sockaddr *) &my_address, &server_len);
    //printf("On port [%d]\n", ntohs(my_address.sin_port));

	//send choice to server
	if (sendto(csfd, choice, strlen(choice)+1, 0, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		error("Error in sendto()\n");
	if (sendto(csfd, &my_address, sizeof(my_address), 0, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		error("Error in sendto() 2\n");


	/* CONNECTION ORIENTED */
	//set up socket for connection oriented communication with group2
	if ((csfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket() 2\n");
	printf("Successfully set up socket.\n");

	//bind
	if (bind(csfd, (struct sockaddr *) &my_address, sizeof(my_address)) < 0) {
		if (shutdown(csfd, SHUT_RDWR) < 0)
			printf("Error in shutdown() 2\n");
		//close(csfd);
		printf("Error in bind() 2\n");
	}
	printf("Address successfully binded to socket.\n");

	//listen for connections
	if ((listen(csfd, MAX_PENDING)) < 0)
		error("Error in listen()\n");
	printf("listen() successful.\n");

	//accept connection
	int nsfd = accept(csfd, (struct sockaddr *) &g2_address, &g2_len);
	char start[100];
	recv(nsfd, start, 100, 0);
	printf("%s\n", start);

	while (1) {
		//send Message
		char buff[100];
		printf("Enter: ");
		scanf("%s", buff);
		send(nsfd, buff, strlen(buff)+1, 0);
		clear(buff);

		//receive message
		recv(nsfd, buff, 100, 0);
		printf("Group2 => %s\n", buff);
		clear(buff);
	}
}
