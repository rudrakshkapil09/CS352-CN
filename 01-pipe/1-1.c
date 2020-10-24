//Assignment 1 - P sends filename to P', P' reads and sends back contents to P, P prints the contents
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define N 1000

//file name is argv[1]
int main(int args, char **argv)
{
	if (args < 2) {
		printf("Run with a filename!\n");
		exit(0);
	}

	//variables to be used
	FILE *fptr;
	char pp1WriteMessage[N] = {'\0'};
	char pp2WriteMessage[N] = {'\0'};
	char readMessage[N] = {'\0'};
	char extension[4] = ".txt";

	//create two pipes for two way communication
	int pp1[2], pp2[2];
	pipe(pp1);	//P to P'
	pipe(pp2);	//P' to P

	//create two processes
	int c = fork();

	//parent - P
	if (c > 0) {
		//close read end of pp1 and write end of pp2
		close(pp1[0]);
		close(pp2[1]);

		//send filename to P'
		strcpy(pp1WriteMessage, argv[1]);	//get name from args
		printf("In P - Filename is %s\n", pp1WriteMessage);
		write(pp1[1], pp1WriteMessage, sizeof(pp1WriteMessage));

		read(pp2[0], readMessage, sizeof(readMessage));
		printf("In P - File Contents:\n%s\n", readMessage);
	}

	//child - P'
	else {
		//close read end of pp2 and write end of pp1
		close(pp2[0]);
		close(pp1[1]);

		//read filename and open file
		read(pp1[0], readMessage, sizeof(readMessage));
		//strcat(readMessage, extension);
		printf("In P' - Filename read: %s\n", readMessage);

		//try to open file
		fptr = fopen(readMessage, "r");
		if (fptr == NULL) {
			printf("Failed to open file!\n");
			exit(0);
		}

		int i = 0;
		char ch = fgetc(fptr);
		while (ch != EOF) {
			pp2WriteMessage[i++] = ch;
			ch = fgetc(fptr);
		}
		fclose(fptr);

		//dummy write:
		printf("In P' - Contents are:\n%s\n", pp2WriteMessage);
		write(pp2[1], pp2WriteMessage, sizeof(pp2WriteMessage));
	}
}
