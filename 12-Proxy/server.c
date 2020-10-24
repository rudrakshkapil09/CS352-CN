//Proxy server
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
#include <pthread.h>
#define MAX_PENDING 20
#define	MAXSIZE 1024
#define NUMS 3

//opens new terminal and runs program
void openNewTerminal(char *program)
{
    char cmd[200], path[100];
    sprintf(cmd, "osascript -e 'tell app \"Terminal\" to do script \"%s/%s\"'", getcwd(path, 100), program);
    system(cmd);
}

//error printing utility function
void error (char *str) {
	perror(str);
	exit(-1);
}

int max(int x, int y)
{
    return (x > y) ? x : y;
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//client details
struct clientDetails {
    int nsfd;
    int serviceNumber;
};
struct clientDetails clients[MAX_PENDING];
int numClients = 0;

//service details
struct serviceDetails {
    int nsfd;
};
struct serviceDetails servers[NUMS];

//global socket file descriptors
int client_sfd;
int clientPort = 8090;	//different clientPorts for different sockets
int server_sfds[NUMS];
int serverPorts[NUMS] = {8091, 8092, 8093};
int proxyService_sfd;
int proxyServicePort = 8089;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");

    shutdown(client_sfd, SHUT_RDWR);	//disallow further reads & writes
    close(client_sfd);
	for (int i = 0; i < NUMS; i++) {
		shutdown(server_sfds[i], SHUT_RDWR);	//disallow further reads & writes
		close(server_sfds[i]);
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

/* Thread Functions - first accepts, the rest is for every server */
void * proxyServerFunction()
{
    //create and read
    fd_set proxyReadFds;

    //maxFD
    int maxFD = max(proxyService_sfd, client_sfd);

    //start select

    while (1) {
        //initialize to 0s
        FD_ZERO(&proxyReadFds);

        //set the fds
        FD_SET(proxyService_sfd, &proxyReadFds);
        FD_SET(client_sfd, &proxyReadFds);

        //select call
        int ret;
        if ((ret = select(maxFD+1, &proxyReadFds, NULL, NULL, NULL)) < 0)
            error("Error in select() in proxy Select\n");

        if (ret == 0)
            continue;

        //input from keyboard
        if (FD_ISSET(proxyService_sfd, &proxyReadFds)) {
            int len;
            struct sockaddr_in address;
            memset(&address, 0, sizeof(address));

            //get nsfd
            int nsfd;
            if ((nsfd = accept(proxyService_sfd, (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
                perror("Error in accept()\n");

            //get choice
            char temp[2];
            recv(nsfd, &temp, 2, 0);
            int choice = atoi(temp);
            printf("Client %d accepted with choice %d.\n", (numClients+1), choice);

            //get message
            char keyboard[100];
            recv(nsfd, &keyboard, 100, 0);

            //swaps E's and A's
            for (int i = 0; keyboard[i] != '\0'; i++) {
                if (keyboard[i] == 'e' || keyboard[i] == 'E')
                    keyboard[i] = 'A';
                else if (keyboard[i] == 'a' || keyboard[i] == 'A')
                    keyboard[i] = 'E';
            }

            //send message back
            printf("[PROXY SERVICE] Converted => %s\n", keyboard);
            send(nsfd, keyboard, 100, 0);
        }

        //input from the client socket
        if (FD_ISSET(client_sfd, &proxyReadFds)) {
            //for use by accept
            int len;
            struct sockaddr_in address;
            memset(&address, 0, sizeof(address));

            //get nsfd
            int nsfd;
            if ((nsfd = accept(client_sfd, (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
                perror("Error in accept()\n");

            //get choice
            char temp[2];
            recv(nsfd, &temp, 2, 0);
            int choice = atoi(temp);

            //store client
            clients[numClients].nsfd = nsfd;
            clients[numClients++].serviceNumber = choice;
            printf("Client %d accepted with choice %d.\n", (numClients+1), choice);
        }
    }

}

void * serverFunction(void *i)
{
    //get server number for this thread
    int server_num = *((int *) i);

    //create and read
    fd_set readfds;

    //start select - input from all clients with serviceNumber == server_num
    while (1) {
        //initialize to 0s
        FD_ZERO(&readfds);

        //time out
        struct timeval tv = {10, 0};

        //set the fds
        int maxFD = 0;
        for (int i = 0; i < numClients; i++) {
            //skip clients not in this server
            if (clients[i].serviceNumber != server_num)
                continue;

            FD_SET(clients[i].nsfd, &readfds);
            maxFD = max(maxFD, clients[i].nsfd);
        }

        //select call
        int ret;
        if ((ret = select(maxFD+1, &readfds, NULL, NULL, &tv)) < 0)
            error("Error in select() in server Select\n");

        if (ret == 0)
            continue;

        //check which client sent
        for (int i = 0; i < numClients; i++) {
            //skip clients not in this server
            if (clients[i].serviceNumber != server_num)
                continue;

            //got input
            if (FD_ISSET(clients[i].nsfd, &readfds)) {
                //receive
                char a[100];
                clear(a);
                recv(clients[i].nsfd, &a, 100, 0);

                //change service
                if (strcmp("CHANGE", a) == 0) {
                    //get choice
                    char temp[2];
                    recv(clients[i].nsfd, &temp, 2, 0);
                    int choice = atoi(temp);

                    printf("Changed server of Client %d from S%d to S%d\n", i, (clients[i].serviceNumber+1), (choice+1));
                    clients[i].serviceNumber = choice;
                    continue;
                }

                //send to server
                printf("Sending to server %d...\n", (i+1));
                send(servers[server_num].nsfd, a, 100, 0);
                //clear(a);

                //receive from server
                recv(servers[server_num].nsfd, &a, 100, 0);

                //send to client
                send(clients[i].nsfd, a, 100, 0);
            }
        }
    }

    free(i);
    return NULL;
}

//MAIN
int main(int argc, char const *argv[])
{
    //set up
    printf("\n---------------------------------------------------------\n");
    printf("PROXY SERVER %d\n", getpid());
    printf("---------------------------------------------------------\n");

	//when program is terminated, clean up socket
	signal(SIGINT,cleanup);

	//create sockets:
    proxyService_sfd = createSocket(proxyServicePort);
    client_sfd = createSocket(clientPort);
	for (int i = 0; i < NUMS; i++) {
        server_sfds[i] = createSocket(serverPorts[i]);
        printf("\nWaiting to accept server %d...\n", (i+1));
        fflush(stdout);

        //for use by accept
        int len;
        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));

        //accept nsfds of services
        int nsfd;
        if ((nsfd = accept(server_sfds[i], (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
            perror("Error in accept()\n");
        printf("Server %d accepted.\n\n", (i+1));

        //store nsfd
        servers[i].nsfd = nsfd;
    }

    //create threads - 1 for getting clients & other proxy servicess + NUMS for each service
    pthread_t accept_tid, server_tid[NUMS];

    pthread_create(&accept_tid, NULL, proxyServerFunction, NULL);
    for (int i = 0; i < NUMS; i++) {
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL)
            error("Error in pthread()");
        *arg = i;
        pthread_create(&server_tid[i], NULL, serverFunction, arg);
    }

    //wait for threads
    pthread_join(accept_tid, NULL);
    for (int i = 0; i < NUMS; i++)
        pthread_join(server_tid[i], NULL);

	return 0;
}
