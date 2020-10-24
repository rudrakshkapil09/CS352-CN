//Bypass server
//Client 7 from group 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#define GROUP2_PORT 8888
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
    printf("\n|  Group 2 Client  |");
    printf("\n+------------------+\n");

    //when program is terminated, clean up sockets
	signal(SIGINT,cleanup);

    //open socket
	if ((csfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error in socket() 1\n");
	printf("Successfully set up socket.\n");

    //set addresses for communication
    struct sockaddr_in my_address, server_address, g1_address;
    socklen_t my_len, server_len, g1_len;
    setAddress(&my_address, GROUP2_PORT);

    //VVIP STEP makes socket reuseble
	int true = 1;
    //assert(setsockopt(csfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int)) == 0);
    setsockopt(csfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int));

    //binding address to socket
    if (bind(csfd, (struct sockaddr *) &my_address, sizeof(my_address)) < 0) {
        if (shutdown(csfd, SHUT_RDWR) < 0)
            error("Error in shutdown() 1\n");
        close(csfd);
        error("Error in bind() 1n");
    }
    printf("Address successfully binded to socket.\n");

    //get address of group1 client
    g1_len = recvfrom(csfd, &g1_address, 100, 0, (struct sockaddr *) &server_address, &server_len);
    printf("Received from port [%d]\n", ntohs(g1_address.sin_port));

    //set up socket for connection oriented communication with group1
	if ((csfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket() 2\n");
	printf("Successfully set up socket.\n");

    //VVIP STEP makes socket reuseble
    true = 1;
    //assert(setsockopt(csfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int)) == 0);
    setsockopt(csfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &true, sizeof(int));

    //bind
	/*
    if (bind(csfd, (struct sockaddr *) &my_address, sizeof(my_address)) < 0) {
        if (shutdown(csfd, SHUT_RDWR) < 0)
            printf("Error in shutdown() 2\n");
        //close(csfd);
        printf("Error in bind() 2\n");
    }
    printf("Address binded to socket.\n");
    sleep(1);
	*/

    //connect
    if (connect(csfd, (struct sockaddr *) &g1_address, g1_len) < 0)
        printf("Error in connect()\n");
    printf("Connection established!\n");

    //display
    char start[100] = "\n--------Connected to P7!--------\n";
    send(csfd, start, strlen(start)+1, 0);

    char start2[100] = "\n--------Connected to Group 1 Client!--------\n";
    printf("%s", start2);

    while (1) {
        //receive message
        char buff[100];
        recv(csfd, buff, 100, 0);
        printf("Group1 => %s\n", buff);
        clear(buff);

        //send Message
        printf("Enter: ");
        scanf("%s", buff);
        send(csfd, buff, strlen(buff)+1, 0);
        clear(buff);
	}
}
