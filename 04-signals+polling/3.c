//Two processes - using signal and shared memory space to hold ID
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int p1ID;
int p2ID;
struct smph * shm;

//shared memory - queue
struct smph
{
	int currID;
	int X;
	int Y;
};


//for p1
void hdfn1() {
	printf("\n[P1 - %d]\n", getpid());
	printf("Read Y -> %d\n", shm->Y);
	printf("Read X -> %d\n", shm->X++);
	printf("New X ----> %d\n", shm->X);

	int pidToSend = shm->currID;
	shm->currID = getpid();
	kill(pidToSend, SIGUSR2);
}

//for p2
void hdfn2() {
	printf("\n[P2 - %d]\n", getpid());
	printf("Read X -> %d\n", shm->X);
	printf("Read Y -> %d\n", shm->Y++);
	printf("New Y ----> %d\n", shm->Y);

	int pidToSend = shm->currID;
	shm->currID = getpid();
	kill(pidToSend, SIGUSR1);
}

//MAIN
int main() {
	//variables
	int shmid, key;

	//get shared memory - print error if necessary
	shmid = shmget(key, sizeof(struct smph), IPC_CREAT|0660);
	if (shmid == -1)
		perror("Shared Memory fault\n");

	//link to shm pointer
	shm = shmat(shmid, NULL, 0);
	if (shm == (void *) -1)
		perror("Attachment fault\n");

	shm->X = 1;
	shm->Y = 1;

	int c = fork();

	//p1
	if (c > 0) {
		signal(SIGUSR1, hdfn1);
		p1ID = getpid();
		printf("P1 => %d\n", p1ID);
		sleep(1);

		//start p1's execution
		raise(SIGUSR1);
		//kill(getpid(), SIGUSR1);
		while(1);
	}
	//p2
	else if (c == 0) {
		signal(SIGUSR2, hdfn2);
		p2ID = getpid();
		printf("P2 => %d\n", p2ID);
		shm->currID = getpid();
		sleep(1);
		while(1);
	}


}

/*
//Two processes - using signal and shared memory space to hold ID
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int p1ID;
int p2ID;
struct smph * shm;

//shared memory - queue
struct smph
{
	int currID;
	int X;
	int Y;
};


//for p1
void hdfn1() {
	//sleep(1);
	printf("\n[P1]\n");
	printf("Read Y -> %d\n", shm->Y);
	printf("Read X -> %d\n", shm->X++);
	printf("New X ----> %d\n", shm->X);
	shm->currID = p2ID;
	kill(shm->currID, SIGUSR2);

	//int pidToSend = shm->currID;
	//shm->currID = getpid();
	//kill(pidToSend, SIGUSR2);
}

//for p2
void hdfn2() {
	//sleep(1);
	printf("\n[P2]\n");
	printf("Read X -> %d\n", shm->X);
	printf("Read Y -> %d\n", shm->Y++);
	printf("New Y ----> %d\n", shm->Y);
	shm->currID = p1ID;
	kill(shm->currID, SIGUSR1);

	//int pidToSend = shm->currID;
	//shm->currID = getpid();
	//kill(pidToSend, SIGUSR2);
}

//MAIN
int main() {
	//variables
	int shmid, key;

	//get shared memory - print error if necessary
	shmid = shmget(key, sizeof(struct smph), IPC_CREAT|0660);
	if (shmid == -1)
		perror("Shared Memory fault\n");

	//link to shm pointer
	shm = shmat(shmid, NULL, 0);
	if (shm == (void *) -1)
		perror("Attachment fault\n");

	shm->X = 1;
	shm->Y = 1;

	int c = fork();

	//p1
	if (c > 0) {
		p1ID = getpid();
		printf("P1 => %d\n", p1ID);
		signal(SIGUSR1, hdfn1);
		sleep(1);

		//start p1's execution
		raise(SIGUSR1);
	}
	//p2
	else if (c == 0) {
		signal(SIGUSR2, hdfn2);
		p2ID = getpid();
		printf("P2 => %d\n", p2ID);
		shm->currID = getpid();
		sleep(1);
	}


}

*/
