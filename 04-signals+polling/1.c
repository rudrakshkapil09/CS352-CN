//Alter signal handler for ^C multiple times
#include <stdio.h>
#include <signal.h>

void hdfn3() {
	printf("Ok enough\n");
	signal(SIGINT, SIG_DFL);
}

void hdfn2() {
	printf("You have pressed ^C again\n");
	signal(SIGINT, hdfn3);
}

void hdfn() {
	printf("You have pressed ^C\n");
	signal(SIGINT, hdfn2);
}

int main() {
	signal(SIGINT, hdfn);
	while(1);
}
