//MultiService Server
//Server
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
#include <sys/select.h>
#include "uds.h"
#define MAX_PENDING 10
#define	MAXSIZE 1024
#define NUMS 2

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

//global socket file descriptors
int usfd;
int sfds[NUMS];
int nusfds[NUMS];
int ports[NUMS] = {8084,8083};	//different ports for different sockets

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	for (int i = 0; i < NUMS; i++) {
		shutdown(sfds[i], SHUT_RDWR);	//disallow further reads & writes
		shutdown(nusfds[i], SHUT_RDWR);
		close(nusfds[i]);
		close(sfds[i]);
	}
	shutdown(usfd, SHUT_RDWR);
	close(usfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//creates socket and returns that sfd
int createSocket(int port)
{
	int sfd;

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//setting up socket address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons((uint16_t) port);	//returns 32 bit integer in host byte order
	address.sin_addr.s_addr = INADDR_ANY;

	//VVIP STEP makes socket reuseble
	int true = 1;
	assert(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) == 0);

	//binding address to server
	if (bind(sfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		if (shutdown(sfd, SHUT_RDWR) < 0)
			error("Error in shutdown()\n");
		close(sfd);
		error("Error in bind()\n");
	}
	printf("Address successfully binded to socket.\n");

	//listening to client requests
	if ((listen(sfd, MAX_PENDING)) < 0)
		error("Error in listen()\n");
	printf("listen() successful.\n");

	return sfd;
}

//MAIN
int main(int argc, char const *argv[])
{
	printf("+--------------------------+\n");
	printf("|      	   Server          |\n");
	printf("+--------------------------+\n");

	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//SET UP UNIX SOCKETS TO SEND CLIENTS TO S1 AND S2
	int usfd;
	struct sockaddr_un userv_addr, ucli_addr;
  	int userv_len, ucli_len;

    //socket
	if ((usfd = socket(AF_UNIX , SOCK_STREAM , 0)) == -1)
	   perror("socket");

    //set address
  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);

    //bind and listen
    userv_len = sizeof(userv_addr);
	if (bind(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
		perror("server: bind");
	listen(usfd, 5);

	//accept unix connections (s1 and s2)
	for (int i = 0; i < NUMS; i++) {
	    ucli_len = sizeof(ucli_addr);
		nusfds[i] = accept(usfd, (struct sockaddr *) &ucli_addr, (socklen_t *) &ucli_len);
	    printf("Accepted S%d!\n", (i+1));
	}

	//create sockets:
	for (int i = 0; i < NUMS; i++)
		sfds[i] = createSocket(ports[i]);

    //create set of fds
    fd_set readfds;

    //find maximum fd
    int maxFD = sfds[0];
    for (int i = 1; i < NUMS; i++)
        if (sfds[i] > maxFD)
            maxFD = sfds[i];

	//start select
	while (1) {
        //initialize readfds to the null set.
        FD_ZERO(&readfds);

        //store the fds of the sockets in readfds
		for (int i = 0; i < NUMS; i++)
			FD_SET(sfds[i], &readfds);

        //get from sockets
		int ret;
		if ((ret = select(maxFD+1, &readfds, NULL, NULL, NULL)) < 0)
			error("Error in select()\n");

		if (ret == 0)
			continue;

        //check each socket to find the one who sent
		for (int i = 0; i < NUMS; i++) {
			if (FD_ISSET(sfds[i], &readfds)) {
				//accept connection request
				int nsfd;
				struct sockaddr_in address;
				int len = sizeof(address);
				if ((nsfd = accept(sfds[i], (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
					perror("Error in accept()\n");
				printf("Connection accepted.\n");

				//send to corresponding Si
				printf("Sending client to S%d...", (i+1));
				send_fd(nusfds[i], nsfd);
			}
		}
	}
	return 0;
}
