//runner
#include "header.h"
const int x = 32;

int sfd;
int nsfds[MAXCLI];
int cli_count = 0;

/* MSGQ stuff */
key_t key;
int msq_id;

struct msg_ds
{
	long msg_type;
	char msg_data[MAXLEN];
} m;

void *clientHandler(void *arg)
{
    int ind = *((int *) arg);
    printf("[%2d] - Handling client\n", ind);

    char buff[MAXLEN];
    while (1) {
        clear(buff);
        recv(nsfds[ind], &buff, sizeof(buff), 0);
        printf("[%2d] - Received => %s\n", ind, buff);

        if (strcmp(buff, "stop") == 0)
            break;

        for (int i = 0; buff[i] != '\0'; i++)
            buff[i] = buff[i] & ~x;

        send(nsfds[ind], buff, strlen(buff), 0);
        printf("S2 -> [%2d] - sent => %s\n", ind, buff);
    }


    printf("S2 -> [%2d] - Done Handling client!\n", ind);
    int s = msgrcv(msq_id, &m, sizeof(m), getpid(), MSG_NOERROR);
    printf("S2 -> [%2d] - got review:\n%s\n\n", ind, m.msg_data);
    return NULL;
}

void newClient()
{
    //for use by accept
    int len;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    //accept connection request
    int nsfd;
    if ((nsfd = accept(sfd, (struct sockaddr*) &address, (socklen_t*) &len)) < 0)
        perror("Error in accept()\n");
    printf("S2 -> Connection accepted.\n");

    nsfds[cli_count] = nsfd;

    //create thread to handle
    pthread_t t;
    int *arg = malloc(sizeof(int *));
    *arg = cli_count;
    pthread_create(&t, NULL, clientHandler, arg);
}

int main()
{
    sfd = dup(0);
    signal(SIGUSR1, newClient);

    //get key of IPC and get message Q id
    if ((key = ftok("./", 100)) == -1)
        error("Error in Path\n");

    //get the queue after clearing old one
    msq_id = msgget(key, 0666 | IPC_CREAT);

    //stay alive
    while (1)
        ;
}
