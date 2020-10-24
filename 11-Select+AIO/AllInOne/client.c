//ALL IN ONE
//Client
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/socket.h>
#define PORT 8091       //must be > 1024
#define MAX_PENDING 10
#define	MAXSIZE 1024

//globals
int echoFlag = 0;
char myfifo2[21] = "/tmp/";
char myfifo3[21] = "/tmp/";
int fd1,fd2, efd;
char buffA[100];
char buffB[100];

//error printing utility function
void error (const char *str) {
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

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//MAIN
int main() {
    //get pid for echoing
    int pid = getpid();
    char pidStr[10] = "";
    sprintf(pidStr, "%d", pid);

    //when program is terminated, clean up socket
	signal(SIGINT, cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	else printf("Successfully set up socket.\n");

	//get host name
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

    /* need to concurrently read and write - select */
    fd_set readfds;
    int echoFlag = 0; //for echo

    char a[100];
	while (1) {
        clear(a);
        //initialize readfds to the null set.
        FD_ZERO(&readfds);

        //store fds of the keyboard and of the socket
        FD_SET(sfd, &readfds);
        FD_SET(0, &readfds);

        //select
        int ret;
        if ((ret = select(sfd+1, &readfds, NULL, NULL, NULL)) < 0)
            error("Error in select()\n");
        else if (ret == 0)
            continue;

        //socket:
        if (FD_ISSET(sfd, &readfds)) {
            recv(sfd, a, 100, 0);
            printf("Received => %s\n", a);
        }
        clear(a);

        //keyboard
        if (FD_ISSET(0, &readfds)) {
            char a[100];
			scanf("%s", a);
            send(sfd, a, strlen(a), 0);
        }
        clear(a);
    }

	return 0;
}
