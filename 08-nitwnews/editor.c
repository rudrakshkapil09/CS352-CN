//NITW NEWS
//Editor
//read messages from fifo and send to news readers
//Opens document writer process if /d is the start
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/shm.h>

//structure for messages
struct msg_ds
{
	long msg_type;
	char msg_data[100];
} m;

//shared memory - queue
struct telecast_smph
{
	int telecasterCount;
};

//globals
char famous_fifo[20] = "/tmp/famous_fifo";
int shmid, msq_id;

//signal handler for SIGUSR1 - increases count of live telecaster
void hdfn1()
{
    shm->telecasterCount++;
    printf("Started a live telecast!\nPresent Number => %d\n", shm->telecasterCount);
}

//signal handler for SIGUSR2 - decreases count of live telecasters
void hdfn2()
{
    shm->telecasterCount--;
    printf("Stopped a live telecast\nPresent Number => %d\n", shm->telecasterCount);
}

//error printing with exit
void err_sys(char *out)
{
	perror(out);
	exit(-1);
}

//to remove first x characters of string s
void removeStart(char *buff, int x)
{
    int i = 0;
    for (; buff[i+x] != '\0'; i++)
        buff[i] = buff[i+x];
    buff[i] = '\0';
}

//MAIN
int main()
{
    //set up
    printf("\n---------------------------------------------------------\n");
    printf("EDITOR %d\nWill receive messages from fifo from reporters\n", getpid());
    printf("---------------------------------------------------------\n");

    //get PID to send to newsReader and set signal handlers
    int pid = getpid();
    signal(SIGUSR1, hdfn1);
    signal(SIGUSR2, hdfn2);

    //set up shared memory for keeping count of telecasters
    key_t shmkey;
    if ((shmkey = ftok("./key/", 100)) == -1)
        err_sys("Error in SHM Key Path\n");
    //get id - print error if necessary
	shmid = shmget(shmkey, sizeof(struct telecast_smph), IPC_CREAT|0660);
	if (shmid == -1)
		perror("Shared Memory fault\n");
	//link to shm pointer
	shm = shmat(shmid, NULL, 0);
	if (shm == (void *) -1)
		perror("Attachment fault\n");
	//initialise pointer
	shm->telecasterCount = 0;

    //make famous fifo
    mkfifo(famous_fifo, 0666);
    int editor_fd = open(famous_fifo, O_RDONLY);  //make blocking - process each thing sequentially

    //make message queue ready by getting key and resetting
    key_t key;
    if ((key = ftok("./", 100)) == -1)
        err_sys("Error in MSGQ Key Path\n");
    msq_id = msgget(key, 0666 | IPC_CREAT);
	msgctl(msq_id, IPC_RMID, NULL);
    msq_id = msgget(key, 0666 | IPC_CREAT);

    //send PID to newsReader with type 2
    m.msg_type = 2;
    sprintf(m.msg_data, "%d", pid);
    assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);

    //continuosly read:
    char news[100], news_for_tokens[100];
    while (1) {
        //get news from fifo
        read(editor_fd, news, 100);
        printf("\nReceived News => %s", news);
        strcpy(news_for_tokens, news);

        //get first word and move forward by that many steps if needed to remove
        char *first = strtok(news_for_tokens, " ");

        //if first word is decimal number it is sent to document writer as well
        if (strcmp(first, "/d") == 0) {
            // 3 because / + d + space
            removeStart(news, 3);
            printf("Sending to document writer...\n");

            //fork and exec the document writer process
            if (fork() == 0)
                execlp("./documentwriter", news, NULL);
        }
        printf("Message to send => %s\n", news);

        //send to news caster (via message queue) with type 1
        m.msg_type = 1;
        sprintf(m.msg_data, "%s", news);
        assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
    }

    printf("Going home...\n");
    system("exit");
    return 0;
}
