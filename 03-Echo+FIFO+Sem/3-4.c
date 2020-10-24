//Semaphores - initialised to 0 0
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define N 10
#define NITERS 40

struct sembuf p = { 0, -1, SEM_UNDO};   // WAIT
struct sembuf v = { 0, +1, SEM_UNDO};   // SIGNAL

//shared memory - queue
struct smph
{
	int X;
	int Y;
};

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

//P1
void *P1(int semid, struct smph *shm)
{
	while (1) {
		printf("P1:\nRead  X = %d\n", shm->X);
		shm->X++;
		printf("P1:\nWrite X = %d\n", shm->X);
		Signal(semid, 0);
		Wait(semid, 1);
		printf("P1:\nRead  Y = %d\n", shm->Y);
		sleep(1);
	}
}

//P2
void *P2(int semid, struct smph *shm)
{
	while (1) {
		Wait(semid, 0);
		printf("P2:\nRead  Y = %d\n", shm->Y);
		shm->Y++;
		printf("P2:\nWrite Y = %d\n", shm->Y);
		sleep(1);
		printf("P2:\nRead  X = %d\n", shm->X);
		Signal(semid, 1);
	}
}

int main()
{
	//variables
	int shmid, semid, key;
	struct smph * shm;

	//get shared memory - print error if necessary
	shmid = shmget(key, sizeof(struct smph), IPC_CREAT|0660);
	if (shmid == -1)
		perror("Shared Memory fault\n");

	//link to shm pointer
	shm = shmat(shmid, NULL, 0);
	if (shm == (void *) -1)
		perror("Attachment fault\n");

	//initialise pointer
	shm->X = 0;
	shm->Y = 0;

	//initialise first semaphore - empty -> N
	union semun u;
	u.val = 0;
	semid = semget(key, 3, 0666 | IPC_CREAT);
	semctl(semid, 0, SETVAL, u);

	//init second - full -> 0
	u.val = 0;
	semctl(semid, 1, SETVAL, u);

	//create processes
	int pid = fork();
	if (pid == 0) {	//child
		//shm = shmat(shmid, NULL, 0);
		P1(semid, shm);
		exit(0);
		wait(NULL);
	}
	else {
		P2(semid, shm);
	}

	return 0;
}
