//NEWS
//News reader - creates two reader processes that read from MQ and print to screen
//Also create the telecast process when required
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

//globals
struct sembuf p = { 0, -1, SEM_UNDO};   // WAIT
struct sembuf v = { 0, +1, SEM_UNDO};   // SIGNAL
int msq_id, editor_pid;
char telecast_fifo[20] = "/tmp/telecast_fifo";

//structure for messages
struct msg_ds
{
	long msg_type;
	char msg_data[100];
} m;

//Wait
void Wait(int semid, int semindex)
{
	p.sem_num = semindex;
	semop(semid, &p, 1);
}

//Signal
void Signal(int semid, int semindex)
{
	v.sem_num = semindex;
	semop(semid, &v, 1);
}

//error printing with exit
void err_sys(char *out)
{
	perror(out);
	exit(-1);
}

//checks if a string is a decimal number
int isNumber(char *buff)
{
    for (int i = 0; buff[i] != '\0'; i++)
        if (!isdigit(buff[i]))
            return 0;
    return 1;
}

//opens new terminal and runs program
void openNewTerminal(char *program)
{
    char cmd[200], path[100];
    sprintf(cmd, "osascript -e 'tell app \"Terminal\" to do script \"%s/%s\"'", getcwd(path, 100), program);
    system(cmd);
}

//starts reading first
void *NewsReader1(int semid)
{
    printf("Good morning from Reader 1!\n");
    while (1) {
        //read from queue
        assert(msgrcv(msq_id, &m, sizeof(m), 1, MSG_NOERROR) == sizeof(struct msg_ds));
        char msg_copy[100];
        strcpy(msg_copy, m.msg_data);

        //check first word - if number start echo server
        char *first = strtok(msg_copy, " ");
        if (isNumber(first)) {
            //inform the editor to increase telecaster count
            kill(editor_pid, SIGUSR1);

            //make fifo for echoing
            mkfifo(telecast_fifo, 0666);

            //open new process
            printf("Here\n");
            openNewTerminal("telecaster");

            //read input and print it out after checking for end
            char livenews[100];
            while (1) {
                int telecast_fd = open(telecast_fifo, O_RDONLY);
                read(telecast_fd, livenews, 100);
                if (strcmp(livenews, "NOTHING\n") == 0) {
                    printf("Reader 1 [TELECAST] => That's all folks!\n");
                    break;
                }
                printf("Reader 1 [TELECAST] => %s", livenews);
                close(telecast_fd);
            }

            //done - remove fifo
            remove(telecast_fifo);

            //inform the editor to decrease telecaster count
            kill(editor_pid, SIGUSR2);
        }
        else
            printf("Reader 1 => %s", m.msg_data);

        //signal reader 2 and wait for 2 to finish
        Signal(semid, 0);
        Wait(semid, 1);
    }
}

//second reader
void *NewsReader2(int semid)
{
    printf("Good morning from Reader 2!\n");
    while (1) {
        //wait till 1 is done
        Wait(semid, 0);
        //print message from queue
        assert(msgrcv(msq_id, &m, sizeof(m), 1, MSG_NOERROR) == sizeof(struct msg_ds));
        char msg_copy[100];
        strcpy(msg_copy, m.msg_data);

        //check first word - if number start echo server
        char *first = strtok(msg_copy, " ");
        if (isNumber(first)) {
            //inform the editor to increase telecaster count
            kill(editor_pid, SIGUSR1);

            //make fifo for echoing
            mkfifo(telecast_fifo, 0666);

            //open new process
            openNewTerminal("telecaster");

            //read input and print it out after checking for end
            char livenews[100];
            while (1) {
                int telecast_fd = open(telecast_fifo, O_RDONLY);
                read(telecast_fd, livenews, 100);
                if (strcmp(livenews, "NOTHING\n") == 0) {
                    printf("Reader 2 [TELECAST] => That's all folks!\n");
                    break;
                }
                printf("Reader 2 [TELECAST] => %s", livenews);
                close(telecast_fd);
            }

            //done - remove fifo
            remove(telecast_fifo);

            //inform the editor to increase telecaster count
            kill(editor_pid, SIGUSR2);
        }
        else
            printf("Reader 2 => %s\n", m.msg_data);

        //tell 1 that 2 is done
        Signal(semid, 1);
    }
}

//MAIN
int main(int argc, char *argv[])
{
    //set up
    printf("\n---------------------------------------------------------\n");
    printf("NEWS READERS\nWill receive messages from editor and print to screen\n");

    //get message queue id
    key_t key;
    if ((key = ftok("./", 100)) == -1)
        err_sys("Error in Path\n");
    msq_id = msgget(key, 0666 | IPC_CREAT);

    //get first message from msq_id -> pid
    assert(msgrcv(msq_id, &m, sizeof(m), 2, MSG_NOERROR) == sizeof(struct msg_ds));
    editor_pid = atoi(m.msg_data);
    printf("{Editor ID => %d}\n", editor_pid);
    printf("---------------------------------------------------------\n");

    //initialise first semaphore - empty
    int semid;
	union semun u;
	u.val = 0;
	semid = semget(key, 3, 0666 | IPC_CREAT);
	semctl(semid, 0, SETVAL, u);

	//init second - full -> 0
	u.val = 0;
	semctl(semid, 1, SETVAL, u);

    //start the two processes
    int c = fork();
    if (c == 0)
        NewsReader1(semid);
    else NewsReader2(semid);

    wait(NULL);
    wait(NULL);
    return 0;
}
