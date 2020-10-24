//Concurrent Connection Oriented Client server
//Server
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#define PORT 8080//must be>1024
#define MAX_PENDING 10
#define	MAXSIZE 1024

//shared memory structure
struct smph
{
	int pollCount;
	struct pollfd fds[10];		//10 is max
};

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

//globals
int sfd;
int shmid;
key_t shmkey;
struct smph *shm;

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
	//set up shared memory for polling
	/*
	if ((shmkey = ftok("./key/", 100)) == -1)
		error("Error in SHM Key Path\n");
	//get id - print error if necessary
	shmid = shmget(shmkey, sizeof(struct smph), IPC_CREAT|0660);
	if (shmid == -1)
		error("Shared Memory fault\n");
	//link to shm pointer
	shm = shmat(shmid, NULL, 0);
	if (shm == (void *) -1)
		error("Attachment fault\n");
	//initialise shared memory
	shm->pollCount = 0;
	shm->fds[0].fd = sfd;
	shm->fds[0].events = POLLIN;
	*/

	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	printf("Successfully set up socket.\n");

	//set socket options - reuseble
	int true = 1;
	int rtn = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
	assert(rtn == 0);

	//setting up socket address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);	//returns 32 bit integer in host byte order
	address.sin_addr.s_addr = INADDR_ANY;

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

	//poll structure
	int pollCount = 1;
	struct pollfd fds[10];		//10 is max
	fds[0].fd = sfd;
	fds[0].events = POLLIN;

	//poll:
	int nsfd;
	int len;
	char buffer[MAXSIZE];
	while (1) {
		int r = poll(fds, pollCount, 2000);
		if (r == 0)								//no message
			continue;
		else {
			//for accepting new clients
			if (fds[0].revents & POLLIN) {
				//accept connection request
				if ((nsfd = accept(sfd, (struct sockaddr *) &address, (socklen_t *) &len)) < 0)
					perror("Error in accept()\n");
				printf("Connection accepted.\n");

				//put nsfd into poll
				fds[pollCount].fd = nsfd;
				fds[pollCount].events = POLLIN;
				pollCount++;
				printf("Current number of clients -> %d\n", (pollCount-1));
			}

			//for checking from clients
			else {
				for (int i = 1; i < pollCount; i++) {
					if (fds[i].revents & POLLIN) {
						printf("From client %d: ", (i));
						read(fds[i].fd, buffer, sizeof(buffer));
						printf("%s", buffer);
						fflush(stdout);
						clear(buffer);
					}
				}
			}
		}
	}
	//child - accept new requests
	/*
	else {
		int len;
		int nsfd;
		while (1) {
			//accept connection request
			if ((nsfd = accept(sfd, (struct sockaddr*) &address, &len)) < 0)
				perror("Error in accept()\n");
			printf("Connection accepted.\n");

			//put nsfd into poll
			shm->fds[shm->pollCount].fd = nsfd;
			shm->fds[shm->pollCount].events = POLLIN;
			shm->pollCount++;
			printf("Current number of clients -> %d\n", shm->pollCount);
		}
	}
	*/

	//fork here - parent polls clients, the child accepts clients
	/*
	int c = fork();

	//parent
	if (c > 0) {
		char buffer[MAXSIZE];
		while (1) {
			int r = poll(shm->fds, shm->pollCount, 2000);
			if (r == 0)								//no message
				continue;
			else {
				for (int i = 0; i < shm->pollCount; i++) {
					if (shm->fds[i].revents & POLLIN) {			//message is there in pipe
						printf("From client %d: ", (i+1));
						read(shm->fds[i].fd, buffer, sizeof(buffer));
						printf("%s", buffer);
						fflush(stdout);
						clear(buffer);
					}
				}
			}
		}
	}

	//child - accept new requests
	else {
		int len;
		int nsfd;
		while (1) {
			//accept connection request
			if ((nsfd = accept(sfd, (struct sockaddr*) &address, &len)) < 0)
				perror("Error in accept()\n");
			printf("Connection accepted.\n");

			//put nsfd into poll
			shm->fds[shm->pollCount].fd = nsfd;
			shm->fds[shm->pollCount].events = POLLIN;
			shm->pollCount++;
			printf("Current number of clients -> %d\n", shm->pollCount);
		}
	}
	*/
	return 0;
}
