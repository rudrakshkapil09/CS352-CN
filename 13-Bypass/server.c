//Bypass Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#define SERVER_PORT 12211
#define MAXSIZE 1024
#define G2_SIZE 1

const int g2_ports[G2_SIZE] = {8888};
const char g2_names[G2_SIZE][2] = {"7"};

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

//used to set address of process
void setAddress(struct sockaddr_in *address, int portNum)
{
    (*address).sin_family = AF_INET;
    (*address).sin_port = htons(portNum);
    (*address).sin_addr.s_addr = INADDR_ANY;
}

//MAIN
int main(int argc, char const *argv[])
{
	printf("\n+------------------+");
    printf("\n|      Server      |");
    printf("\n+------------------+\n");

    //when program is terminated, clean up sockets
	signal(SIGINT,cleanup);

	//open socket for Group 1
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//setting up socket address
	struct sockaddr_in server_address;
	setAddress(&server_address, SERVER_PORT);

	//VVIP STEP makes socket reuseble
	int true = 1;
	//assert(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int)) == 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int));

	//binding address to socket
	if (bind(sfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
		error("Error in bind()\n");
	printf("Address successfully binded to socket.\n");

	//start server
    char buff[MAXSIZE];
    while (1) {
		struct sockaddr_in recv_address,to_address, actual_address;
        socklen_t recv_len = sizeof(recv_address);

		recvfrom(sfd, buff, MAXSIZE, 0, (struct sockaddr *) &recv_address, &recv_len);
		printf("Received => %s\n", buff);
		printf("%d\n\n", ntohs(recv_address.sin_port));

		recvfrom(sfd, &actual_address, sizeof(recv_address), 0, (struct sockaddr *) &recv_address, &recv_len);
		printf("Actual port => %d\n\n", ntohs(actual_address.sin_port));

		//check all group 2 for match
		for (int i = 0; i < G2_SIZE; i++) {
			//match
			if (strcmp(buff, g2_names[i]) == 0) {
				setAddress(&to_address, g2_ports[i]);
				printf("Sending request to %s in Group 2 - [%d]...\n", g2_names[i], ntohs(actual_address.sin_port));
				sendto(sfd, &actual_address, sizeof(actual_address), 0, (struct sockaddr *) &to_address, sizeof(to_address));
			}
		}
	}
	return 0;

}
