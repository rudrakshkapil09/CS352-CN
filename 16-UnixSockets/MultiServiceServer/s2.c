//Service 2 - Converts to Lowercase
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "uds.h"
const int x = 32;

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//sockets
int usfd;
int nsfds[10];
int nsfdsCount = 0;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	for (int i = 0; i < nsfdsCount; i++) {
		shutdown(nsfds[i], SHUT_RDWR);	//disallow further reads & writes
		close(nsfds[i]);
	}
	shutdown(usfd, SHUT_RDWR);
	close(usfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//MAIN
int main()
{
	printf("+--------------------------+\n");
	printf("|            S2            |\n");
	printf("+--------------------------+\n");

	//when program is terminated, clean up sockets
	signal(SIGINT,cleanup);

	//CONNECT TO SERVER VIA N
	int usfd;
	struct sockaddr_un userv_addr;
	int userv_len, ucli_len;

    //socket
	if ((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	   perror("\nsocket  ");

    //set socket address
	bzero(&userv_addr,sizeof(userv_addr));
	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);  //put ADDRESS in path
	userv_len = sizeof(userv_addr);

    //connect to socket
	if (connect(usfd, (struct sockaddr *)&userv_addr, userv_len) < 0)
	   perror("\n connect ");
	else printf("Connected successfully to server!\n");

	//SET UP SELECT - for all nsfd to service and usfd for accepting new clients
	//create set of fds
	fd_set readfds;

	char buff[100];
	while (1) {
		//initialize readfds to the null set.
		FD_ZERO(&readfds);

		//store the nsfds & usfd in readfds & find maxx
		int maxFD = usfd;
		FD_SET(usfd, &readfds);
		for (int i = 0; i < nsfdsCount; i++) {
			maxFD = max(nsfds[i], maxFD);
			FD_SET(nsfds[i], &readfds);
		}

		//get from sockets
		int ret;
		if ((ret = select(maxFD+1, &readfds, NULL, NULL, NULL)) <= 0)
			continue;

		//accept clients
		if (FD_ISSET(usfd, &readfds)) {
			nsfds[nsfdsCount++] = recv_fd(usfd);
			printf("Added client!\nCount => %d\n\n", nsfdsCount);
		}

		//service clients - read, alter, send
		for (int i = 0; i < nsfdsCount; i++) {
			if (FD_ISSET(nsfds[i], &readfds)) {
				printf("Servicing Client [%d]...\n", i);

				clear(buff);
				recv(nsfds[i], &buff, sizeof(buff), 0);
				printf("Received => %s\n", buff);

				for (int j = 0; buff[j] != '\0'; j++)
					buff[j] = buff[j] & ~x;

				send(nsfds[i], buff, strlen(buff), 0);
				printf("Sent => %s\n\n", buff);
			}
		}
	}
}
