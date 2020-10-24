//Chat Server with message queue
//Client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

//structure for messages
struct msg_ds
{
	long msg_type;
	char msg_data[100];
} m;

//error printing with exit
void err_sys(char *out)
{
	perror(out);
	exit(-1);
}

//globals
key_t key;
int msq_id, t;
pid_t pid;
pthread_t rd,wt;

//reader thread for t
void* readt(void *arg)
{
	struct msg_ds r;
	while (1) {
		//assert(msgrcv(msq_id, &m, sizeof(m), t, 0) == sizeof(struct msg_ds));
		int s = msgrcv(msq_id, &m, sizeof(m), t, MSG_NOERROR);
		//printf("{%d} = [%ld] - %s\n", s, m.msg_type, m.msg_data);
		printf("%s\n", m.msg_data);
		sleep(5);
	}
}

//writer thread for user
void* writeu(void *arg)
{
	int count = 0;
	while (1) {
		//get input
		char input[90];
		fgets(input, 90, stdin);

		//if input is exit command
		if (strcmp(input, "###") == 0) {
			sprintf(m.msg_data, "### %d ", pid);
			m.msg_type = 1;
			assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
			pthread_exit(0);
		}

		//otherwise send to server with type 1. Format: pid message
		sprintf(m.msg_data, "%d %s", getpid(), input);
		m.msg_type = 1;
		assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
		sleep(2);
		count++;
	}
}

//MAIN
int main(int argc, char **argv)
{
	printf("\n-------------------\nClient %d\n-------------------\n", getpid());

	//get key of IPC and get message Q id
	key = ftok("./", 100);
	msq_id = msgget(key, 0666 | IPC_CREAT);
	printf("msq_id: \n%d\n", msq_id);

	//get unique identifier of this process
	pid = getpid();
	t = (long) pid;

	//for joining the chat
	m.msg_type = 1;
	sprintf(m.msg_data, "*** %d ", pid);
	assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
	printf("Connected to Server\n\n");

	pthread_create(&wt, NULL, writeu, NULL);
	pthread_create(&rd, NULL, readt, NULL);

	//wait for them to end
	pthread_join(wt, NULL);
	pthread_join(rd, NULL);
	return 0;
}
