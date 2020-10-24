//Chat Server with message queue
//Server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <assert.h>
#include <time.h>

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

//MAIN
int main(int argc, char **argv)
{
	printf("\n-------------------\n\tServer\n-------------------\n");

	key_t key;
	int msq_id;

    //get key of IPC and get message Q id
	if ((key = ftok("./", 100)) == -1)
        err_sys("Error in Path\n");

	//get the queue after clearing old one
	msq_id = msgget(key, 0666 | IPC_CREAT);
	msgctl(msq_id, IPC_RMID, NULL);
	msq_id = msgget(key, 0666 | IPC_CREAT);
	printf("Message Queue Created\nID: %d\n", msq_id);

	long pids[10];
	int pcount = -1;
	time_t prevTime;

	while (1) {
		//parent is listening for messages of type 1
		int size;
		assert((size = msgrcv(msq_id, &m, sizeof(m), 1, MSG_NOERROR)) == sizeof(struct msg_ds));
		//msgrcv(msq_id, &m, sizeof(m), 1, MSG_NOERROR);
		printf("\nRecieved: %s\n", m.msg_data);

		//get first token to determine next step taken
		char *tok;
		tok = strtok(m.msg_data, " ");
		printf("Recieved from: %s\n", tok);

		//if process joins the chat
		if (strcmp(tok, "***") == 0) {
			if (pcount == -1)
				pcount = 0;

			//get pid and add it to pids array
			tok = strtok(NULL, " ");
			pids[pcount] = atoi(tok);
			printf("%ld was added to the chat\n", pids[pcount]);
			pcount++;
			continue;
		}
		//if process leaves the chat
		else if ((strcmp(tok,"###") == 0) || tok[0] == '#')
		{
			//get pid of client to remove
			tok = strtok(NULL, " ");
			long rm = atol(tok);		//USE atol
			printf("Here\n");

			//remove said client
			for (int i = 0; i < pcount; i++) {
				if (pids[i] == rm) {
					//shift left
					for (int j = i; j < pcount-1; j++)
						pids[j] = pids[j+1];
					//reduce count
					pcount--;
					printf("PID: %ld has left the chat\n",rm);
				}
			}

			//check if no more clients
			if (pcount <= 0)
				break;
			else continue;
		}
		//if its a message
		else if (isdigit(tok[0]))
		{
			//get sender pid and message
			long sender = atoi(tok);
			tok = strtok(NULL, "\n");
			printf("%s\n", tok);

			for (int i = 0; i < pcount; i++) {
				if (pids[i] != sender) {
					printf("Sending \"%s\" to %ld\n", tok, pids[i]);
					//struct msg_ds snd;
					//snd.msg_type = pids[i];
					//sprintf(snd.msg_data, "%ld => %s", sender, tok);
					//assert(msgsnd(msq_id, &snd, sizeof(snd), 0 /* flag NOT message type*/) == 0);
					m.msg_type = pids[i];
					sprintf(m.msg_data, "%ld %s", sender, tok);
					assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
				}
			}
		}

		while (tok != NULL)
			tok = strtok(NULL, " ");
	}

	// to destroy the message queue
	printf("Chat server is empty. Destroying message queue...\n");
	msgctl(msq_id, IPC_RMID, NULL);
	return 0;
}
