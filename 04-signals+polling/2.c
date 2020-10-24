//Two processes communicating with signals
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void hdfn1() {
	printf("Received a signal from P 2\n");
}

void hdfn2() {
	printf("Received a signal from P1\n");
}

//MAIN
int main() {
	int c = fork();
	signal(SIGUSR1, hdfn1);
	signal(SIGUSR2, hdfn2);

	if (c > 0) {
		sleep(1);
		while (1) {
			kill(c, SIGUSR1);
			sleep(1);
		}
	}
	else if (c == 0) {
		sleep(1);
		while(1) {
			kill(getppid(), SIGUSR2);
			sleep(1);
		}
	}

}
