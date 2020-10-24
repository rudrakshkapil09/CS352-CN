//Iterative Connection Oriented Client server
//Client
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define PORT 12082
#define	MAXLEN 1024
const int x = 32;

//structure for messages
struct msg_ds
{
	long msg_type;
	char msg_data[100];
} m;

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
int ffd;
int waiting = 0;
char fifoname[100] = "./fifo";

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(sfd, SHUT_RDWR);	//disallow further reads & writes
	close(sfd);
	printf("Cleaned sockets!\n");
	exit(0);
}


//utility for SIGUSR1 - used to break busy waiting loop
void stopWaiting()
{
	waiting = 0;
	close(ffd);
}

//MAIN
int main(int argc, char const *argv[])
{
	//start display
	printf("\n+------------------+");
	printf("\n|   Client %d   |", getpid());
	printf("\n+------------------+\n");

	//when program is terminated, clean up socket
	signal(SIGINT, cleanup);
	signal(SIGUSR1, stopWaiting);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	else printf("Successfully set up socket.\n");

	//get host name - because ipv4 is specified, function copies name into the h_name field
	//and its struct in_addr equivalent into the h_addr_list[0] field of the returned hostent struct
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

	/* AT THIS POINT WE HAVE SFD */
	//set up message queue
	key_t key;
	int msq_id;

	//get key of IPC and get message Q id
	if ((key = ftok("./", 100)) == -1)
		error("Error in Path\n");

	//get the queue after clearing old one
	msq_id = msgget(key, 0666 | IPC_CREAT);
	msgctl(msq_id, IPC_RMID, NULL);
	msq_id = msgget(key, 0666 | IPC_CREAT);
	printf("Message Queue Created\nID: %d\n", msq_id);

	//make fifo for communication with others
	mkfifo(fifoname, 0666);

	//inf loop
	char buff[MAXLEN];
	while (1) {
		//process others while
		while (waiting) {
			//take input from fifo, send to ESS, get back and send it back through fifo
			printf("Servicing some other client...\n");
			ffd = open(fifoname, O_RDONLY);
			read(ffd, buff, sizeof(buff));
			if (buff[0] == '0')
				break;
			close(ffd);

			printf("Received from fifo => %s\n", buff);
			if (send(sfd, buff, strlen(buff), 0) < 0)
				printf("Error in send()\n");
			clear(buff);

			//receive message
			while (1) {
				int n;
				if ((n = recv(sfd, &buff, sizeof(buff), 0)) < 0)
					printf("Error in recv()\n");
				else if (n > 0) {
					printf("Received => %s\n", buff);
					break;
				}
			}

			//send back
			ffd = open(fifoname, O_WRONLY);
			write(ffd, buff, strlen(buff));
			close(ffd);
			clear(buff);
		}
		waiting = 1;	//done waiting for next loop

		//continuously send and receive until done
		printf("\nYou can now start using ESS\nEnter 0 to stop\n------------------\n");
		while (1) {
			//send message
			printf("Enter: ");
			scanf("%s", buff);
			if (strcmp("0", buff) == 0)
				break;
			send(sfd, buff, strlen(buff), 0);
			clear(buff);

			//receive messag
			while (1) {
				int n;
				if ((n = recv(sfd, &buff, sizeof(buff), 0)) < 0)
					error("Error in recv()\n");
				else if (n > 0) {
					printf("Received => %s\n", buff);
					break;
				}
			}
			clear(buff);
		}

		//done - add itself to back of msg queue
		printf("------------------\nDone using ESS\n");
		m.msg_type = getpid();
		strcpy(m.msg_data, "Hello!");
		assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);

		//get next and signal to them
		int size;
		assert((size = msgrcv(msq_id, &m, sizeof(m), 0, MSG_NOERROR)) == sizeof(struct msg_ds));
		printf("Next client => %ld\n\n", m.msg_type);
		kill(m.msg_type, SIGUSR1);
	}



	return 0;
}
