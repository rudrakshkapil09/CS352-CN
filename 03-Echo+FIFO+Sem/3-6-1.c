//Online Judge
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
		FILE *rptr, *op1_ptr, *op2_ptr;
		rptr = fopen("3-6-test.txt", "r");
		op1_ptr = fopen("3-6-op1.txt", "r+");
		op2_ptr = fopen("3-6-op2.txt", "r+");

		int rfd = fileno(rptr);
		int op1_fd = fileno(op1_ptr);
		int op2_fd = fileno(op2_ptr);

		int stdin_fd = dup(0);
		int stdout_fd = dup(1);

		dup2(rfd, 0);
		dup2(op1_fd, 1);

		//get standard op
		int c = fork(), status;
		if (c == 0)	//child -> run and exit
				execlp("./3-6-2", "./3-6-2", NULL);
		else waitpid(c, &status, 0);

		//get user op
		dup2(op2_fd, 1);	//change op file first and reset ip file
		fseek(rptr, 0, SEEK_SET);
		int d = fork();
		if (d == 0)
			//execlp("./3-6-3", "./3-6-3", NULL);
			execlp("./3-6-3", NULL, NULL);
		else waitpid(d, &status, 0);

		//close all open files
		dup2(stdout_fd, 1);
		dup2(stdin_fd, 0);
		close(stdin_fd); close(stdout_fd);
		close(op1_fd); close(op2_fd);
		close(rfd); fclose (op1_ptr); fclose(op2_ptr); fclose(rptr);

		//check and print if same or not
		op1_ptr = fopen ("3-6-op1.txt", "r");
		op2_ptr = fopen ("3-6-op2.txt", "r");

		int i = 0;
		char ch;
		char op1[100];
		if (op1_ptr != NULL) {
				while ((ch = fgetc(op1_ptr)) != EOF)
						op1[i++] = ch;
				op1[i] = '\0';
				fclose(op1_ptr);
		}

		i = 0;
		char op2[100];
		if (op2_ptr != NULL) {
				while ((ch = fgetc(op2_ptr)) != EOF)
						op2[i++] = ch;
				op2[i] = '\0';
				fclose(op2_ptr);
		}

		if (strcmp(op1, op2) != 0)
				printf("Wrong!\nYour Output => %s\nExpected Output => %s", op2, op1);
		else 	printf("Correct!\nYour Output => %s\nExpected Output => %s\n", op2, op1);
		return 0;
}
