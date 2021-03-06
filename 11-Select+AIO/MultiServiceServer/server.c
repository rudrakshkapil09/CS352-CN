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
int sfds[NUMS];
int ports[NUMS] = {8084,8083};	//different ports for different sockets

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	for (int i = 0; i < NUMS; i++) {
		shutdown(sfds[i], SHUT_RDWR);	//disallow further reads & writes
		close(sfds[i]);
	}
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
	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

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

/*
	//create poll and store sfds
	struct pollfd fds[NUMS];		//10 is max
	for (int i = 0; i < NUMS; i++)
	{
		fds[i].fd = sfds[i];
		fds[i].events = POLLIN;
	}
*/
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
				if ((nsfd = accept(sfds[i], (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
					perror("Error in accept()\n");
				printf("Connection accepted.\n");

				//fork
				int c = fork();
				if (c < 0)
					error("Error in fork()");
				else if (c > 0)
					close(nsfd);	//parent closes nsfd
				else {
					//close sfds
					for (int j = 0; j < NUMS; j++)
						close(sfds[j]);

                    if (i == 0)
                        printf("Opening s1\n");
                    else printf("Opening s2\n");

					//dup so we dont lose nsfd
					dup2(nsfd, 0);
					dup2(nsfd, 1);

					//select which service depending on sockets
					switch (i) {
						case 0: execlp("./s1", "./s1", NULL);
						case 1: execlp("./s2", "./s2", NULL);
						default: error("INVALID SWITCH");
					}
				}
			}
		}
	}
	return 0;
}
