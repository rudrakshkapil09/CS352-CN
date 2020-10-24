//Service 2 - Converts to Uppercase
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
const int x = 32;
const int MAX_CLIENTS = 5;

//globals
int numClients = 0;
int nsfds[MAX_CLIENTS];

//utility
int max(int x, int y)
{
	return (x < y) ? y : x;
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//MAIN
int main()
{
	//retrieve sockets
	int sfd = dup(0);
	int nsfd = dup(1);
	nsfds[numClients++] = nsfd;

	//send confirmation to client
	char buff[100] = "First connection to server 2!";
	send(nsfd, buff, strlen(buff), 0);
	clear(buff);

	//use select to check for all
	fd_set readfds;

	//loop
	while (1) {
		//initialse
		FD_ZERO(&readfds);

		//add fds and find max
		int maxFD = sfd;
		FD_SET(sfd, &readfds);
		for (int i = 0; i < numClients; i++) {
			FD_SET(nsfds[i], &readfds);
			maxFD = max(maxFD, nsfds[i]);
		}

		int ret = select(maxFD+1, &readfds, NULL, NULL, NULL);
		if (ret <= 0)
			continue;

		//from clients
		for (int i = 0; i < numClients; i++) {
			if (FD_ISSET(nsfds[i], &readfds)) {
				clear(buff);
				recv(nsfds[i], &buff, sizeof(buff), 0);

				for (int i = 0; buff[i] != '\0'; i++)
					buff[i] = buff[i] & ~x;

				send(nsfds[i], buff, strlen(buff), 0);
			}
		}

		//accepting new clients
		if (FD_ISSET(sfd, &readfds)) {
			//for use by accept
			int len;
			struct sockaddr_in address;
			memset(&address, 0, sizeof(address));

			//accept connection request
			if ((nsfd = accept(sfd, (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
				perror("Error in accept()\n");

			//5 is max
			if (numClients >= MAX_CLIENTS) {
				char error[100] = "Sorry, already got 5 clients!";
				send(nsfd, error, strlen(error), 0);
				continue;
			}

			//send accepted and store nsfd
			char msg[100] = "Connected to server 2!";
			send(nsfd, msg, strlen(msg), 0);
			clear(msg);
			nsfds[numClients++] = nsfd;

			//signal main server if reached MAX CLIENTS
			if (numClients == MAX_CLIENTS)
				kill(getppid(), SIGUSR1);
		}
	}
 	return 0;
}
