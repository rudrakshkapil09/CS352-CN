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
#include <sys/ipc.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/msg.h>
#define	MAXLEN 1024

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
int waiting = 1;
char fifoname[100] = "./fifo";

//utility for SIGUSR1 - used to break busy waiting loop
void stopWaiting()
{
	waiting = 0;
}

//MAIN
int main(int argc, char const *argv[])
{
	//start display
	printf("\n+------------------+");
	printf("\n|   Client %d   |", getpid());
	printf("\n+------------------+\n");

	//when program is terminated, clean up socket
	signal(SIGUSR1, stopWaiting);

	//set up message queue
	key_t key;
	int msq_id;

	//get key of IPC and get message Q id
	if ((key = ftok("./", 100)) == -1)
		error("Error in Path\n");

	//get the queue after clearing old one
	msq_id = msgget(key, 0666 | IPC_CREAT);
	printf("Message Queue Retrieved\nID: %d\n", msq_id);

	//send itself
	m.msg_type = getpid();
	sprintf(m.msg_data, "Hello!");
	assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);

	//make fifo
	mkfifo(fifoname, 0666);

	//inf loop
	while (1) {
		//busy waiting
		while (waiting)
			;
		waiting = 1;

		//continuously send and receive until done
		printf("\nYou can now start using ESS\nEnter 0 to stop\n------------------\n");
		char buff[MAXLEN];
		while (1) {
			//send message
			printf("Enter: ");
			scanf("%s", buff);
			if (strcmp("0", buff) == 0)
				break;
			int ffd = open(fifoname, O_RDWR);
			write(ffd, buff, strlen(buff));
			clear(buff);
			sleep(1);
			read(ffd, buff, 100);
			printf("Recieved => %s\n", buff);
			clear(buff);
			close(ffd);
		}

		//done - add itself to back of msg queue
		printf("------------------\nDone using ESS\n");
		m.msg_type = getpid();
		sprintf(m.msg_data, "Hello!");
		assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);

		//get next and signal to them
		int size;
		assert((size = msgrcv(msq_id, &m, sizeof(m), 0, MSG_NOERROR)) == sizeof(struct msg_ds));
		printf("Next client => %ld\n\n", m.msg_type);
		kill(m.msg_type, SIGUSR1);
	}

	return 0;
}
