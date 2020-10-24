// ALL IN ONE
// Server
// Contains code for p1 as well
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

//structs
struct nsfdsStruct {
	int nsfd;
	int echoPID;
	int echoFlag;
};

//globals
char myfifo1[21] = "/tmp/";
char pfifo[21] = "/tmp/pfifo";
char ppfifo[21] = "/tmp/ppfifo";
int nsfds_count = 0;
struct nsfdsStruct nsfds[MAX_PENDING];
int s = 0;
int sfd;

//utility function
int max(int x, int y)
{
	return (x > y) ? x : y;
}

//error printing utility function
void error (const char *str) {
	perror(str);
	exit(-1);
}

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

//handler for SIGUSR1 - sent by p4
void hdfn(int signo) {
	printf("[GOT SIGUSR1]\n");

	//look for first client without an echo
	int i = 0;
	for (; i < nsfds_count; i++)
		if (nsfds[i].echoFlag == 0)
			break;

	//check if we found a valid client
	if (i == nsfds_count) {
		printf("Could not open echo!\n");
		return;
	}
	else printf("Starting echo with client %d...\n", i);

	//update structure
	nsfds[i].echoFlag = 1;

	//store server pid to pass as argument
	char s[20];
	sprintf(s, "%d", getpid());

	int c = fork();
	if (c == 0)
	{
		//dup and exec after storing child pid
		nsfds[i].echoPID = getpid();
		dup2(nsfds[i].nsfd, 0);
		dup2(nsfds[i].nsfd, 1);
		execlp("./echo", s, NULL);
	}
	else if (c > 0) {
		//store child pid
		nsfds[i].echoPID = c;
	}

	printf("Client %d has echo %d!\n", i, nsfds[i].echoPID);
}

//remove echo server after signal from server
void removeEcho(int sender_pid) {
	printf("Removing echo...\n");

	//loop through clients to find matching echoPID
	for (int i = 0; i < nsfds_count; i++) {
		if (nsfds[i].echoPID == sender_pid) {
			nsfds[i].echoFlag = 0;
			printf("Echo for client %d removed!\n", i);
			break;
		}
	}
}

//signal handler for SIGUSR2
void get_pid(int sig, siginfo_t *info, void *context)
{
    int signalPid = info->si_pid;
	removeEcho(signalPid);
}

//MAIN
int main() {
    int pfd[2];
    pipe(pfd);
    int c = fork();

    //server
    if (c > 0) {
        //when program is terminated, clean up socket
    	signal(SIGINT, cleanup);
        signal(SIGUSR1, hdfn);  // for p4
		//signal(SIGUSR2, removeEcho);

		//sigaction for removing echo server
		struct sigaction sa;
		sa.sa_flags = SA_SIGINFO;
    	sa.sa_sigaction = get_pid;
		sigaction(SIGUSR2, &sa, NULL);

    	//open socket
    	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    		error("Error in socket()\n");
    	printf("Successfully set up socket.\n");

    	//set socket options
    	int trued = 1;
    	int rtn = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &trued, sizeof(int));
    	assert(rtn == 0);

    	//setting up socket address
    	struct sockaddr_in address;
    	memset(&address, 0, sizeof(address));
    	address.sin_family = AF_INET;
    	address.sin_port = htons(PORT);	//returns 32 bit integer in host byte order
    	address.sin_addr.s_addr = INADDR_ANY;

		//for use by select()
		struct timeval t;
		t.tv_sec = 50;
		t.tv_usec = 0;

    	//binding address to server
    	if (bind(sfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
    		if (shutdown(sfd, SHUT_RDWR) < 0)
    		      error("Error in shutdown()\n");
    		close(sfd);
    		error("Error in bind()\n");
    	}
    	printf("Address successfully bound to socket\n");

    	//listening to client requests
    	if ((listen(sfd, MAX_PENDING)) < 0)
    		error("Error in listen()\n");
    	printf("listen() successful.\n");

        //set up select
        fd_set readfds;

        //make fifos and pipe handling
        close(pfd[1]);
        mkfifo(pfifo, 0666);
        mkfifo(ppfifo, 0666);

		//put pid of server into ppfifos
		int serfd = open(ppfifo, O_WRONLY);
		int p = (int) getpid();
		char c[100];
		sprintf(c, "%d", p);
		write(serfd, c, strlen(c)+1);
		printf("Writing PID done: %s\n", c);
		//close(serfd);

        //set up fds
        int ffd = open(pfifo, O_RDONLY | O_NONBLOCK);
        int fd = fileno(popen("./p3", "r"));

        //get inputs
    	int nsfd;
        char a[100];
        printf("Here\n");
    	while (1) {
            //clear
            clear(a);

            //initialize readfds to the null set.
            FD_ZERO(&readfds);

            //store fds of the different processes and of the socket
            FD_SET(sfd, &readfds);
            FD_SET(0, &readfds);
            FD_SET(pfd[0], &readfds);
            FD_SET(ffd, &readfds);
            FD_SET(fd, &readfds);

            //store nsfds of clients as well
            for (int i = 0; i < nsfds_count; i++)
                FD_SET(nsfds[i].nsfd, &readfds);

            //find maximum
            int maxFD = max(sfd, max(pfd[0], max(ffd, fd)));
            for (int i = 0; i < nsfds_count; i++)
                maxFD = max(maxFD, nsfds[i].nsfd);

			//select
    		int ret;
			if ((ret = select(maxFD+1, &readfds, NULL, NULL, &t)) < 0) {
    			printf("Error in select()\n");
				continue;
			}
            else if (ret == 0) {
                printf("Returned 0");
                continue;
            }

            /*check each:*/
            //keyboard input
            if (FD_ISSET(0, &readfds)) {
				scanf("%s", a);

                //send message to clients
                for (int i = 0; i < nsfds_count; i++) {
					//skip those being served by echo
					if (nsfds[i].echoFlag == 1)
						continue;

                    send(nsfds[i].nsfd, a, strlen(a), 0);
                    printf("Sent '%s' By Keyboard\n", a);
                }
            }
            clear(a);

            //New client coming in via socket
            if (FD_ISSET(sfd, &readfds)) {
                //for accept
                int len;
                struct sockaddr_in address;
                memset(&address, 0, sizeof(address));

                //accept connection request
                if ((nsfd = accept(sfd, (struct sockaddr*) &address, (socklen_t *) &len)) < 0)
                    perror("Error in accept()\n");
                printf("Connection accepted.\n");

                //store nsfd to communicate with client
				nsfds[nsfds_count].nsfd = nsfd;
				nsfds[nsfds_count].echoFlag = 0;
				nsfds_count++;
            }
            clear(a);

            //P1:
            if (FD_ISSET(pfd[0], &readfds)) {
                read(pfd[0], a, 100);

                //send message to clients
                for (int i = 0; i < nsfds_count; i++) {
					//skip those being served by echo
					if (nsfds[i].echoFlag == 1)
						continue;

                    send(nsfds[i].nsfd, a, strlen(a), 0);
					printf("Sent '%s' By P1\n", a);
                }
            }
            clear(a);

            //P2:
            if (FD_ISSET(ffd, &readfds)) {
                read(ffd, a, 100);

                //send message to clients
                for (int i = 0; i < nsfds_count; i++) {
					//skip those being served by echo
					if (nsfds[i].echoFlag == 1)
						continue;

                    send(nsfds[i].nsfd, a, strlen(a), 0);
					printf("Sent '%s' By P2\n", a);
                }
            }
            clear(a);

            //P3:
            if (FD_ISSET(fd, &readfds)) {
                read(fd, a, 100);

                //send message to clients
                for (int i = 0; i < nsfds_count; i++) {
					//skip those being served by echo
					if (nsfds[i].echoFlag == 1)
						continue;

                    send(nsfds[i].nsfd, a, strlen(a), 0);
					printf("Sent '%s' By P3\n", a);
                }
            }
            clear(a);

            //Clients (only those without echo):
            for (int i = 0; i < nsfds_count; i++) {
                if (FD_ISSET(nsfds[i].nsfd, &readfds) && nsfds[i].echoFlag == 0) {
                    //read
                    recv(nsfds[i].nsfd, &a, 100, 0);

                    //send to all other clients that don't have an echo
                    for (int j = 0; j < nsfds_count; j++)
                        if (i != j && nsfds[j].echoFlag == 0)
                            send(nsfds[j].nsfd, a, strlen(a), 0);
                }
                clear(a);
            }
    	}
    }
    else { //P1 program
        close(pfd[0]);
        while (1) {
            sleep(20);
            char a[100] = "Message from P1";
            write(pfd[1], a, sizeof(a));
            clear(a);
        }
    }
    return 0;
}
