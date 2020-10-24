//Echo Server - P1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>

//clears an array
void clear(char c[],int n)
{
		for (int i = 0; i < n; i++)
			c[i] = '\0';
}

//MAIN
int main()
{
	int pp1[2];
    int i = 3;

    if (pipe(pp1) == -1)
				printf("\nError Creating the Pipe");

    int oldradd = dup(0);
    dup2(pp1[0], 0);
	
    while (1) {
      	int c = fork();
        if ( c > 0 ) //Parent Process
      	{
						printf("\n-----------[P1]------------\n");
            char c1[100];
           	dup2(oldradd,0);
           	printf("Enter the Message to Send to Process P2: ");
           	scanf("%s",c1);
           	char c2[100] = "#";
           	char c3[100];
           	snprintf(c3, 10, "%d", pp1[1]);
           	dup2(pp1[0], 0);
           	write(pp1[1], strcat(c1, strcat(c2, c3)), 100);
           	wait(NULL);
           	char c5[100];
           	clear(c5, 100);
           	read(0, c5, 100);
						printf("\n-----------[P1]------------\n");
           	printf("%s",c5);
        }
        else //Child Process - run P2
            execlp("./3-1-2","./3-1-2",NULL);
    }
}
/*
int main()
{
	fputs("[P1]\n", stdout);

	FILE* wfp = popen("./3-1-2", "w");
	FILE* rfp = popen("./3-1-2", "r");

	int w_fd = fileno(wfp);
	int r_fd = fileno(rfp);

	int stdout_fd = dup(1);
	int stdin_fd = dup(0);

	//get input
	char buff[MAX_LENGTH];
	gets(buff);

	//send to P1
	dup2(w_fd, 1);
	fputs(buff, stdout);
	fflush(stdout);

	//get message from p2
	char buff2[MAX_LENGTH];
	dup2(r_fd, 0);
	fgets(buff2, MAX_LENGTH, stdin);

	//replace original 0 and 1
	dup2(stdout_fd, 1);
	dup2(stdin_fd, 0);

	//print
	fputs("[P1]\n", stdout);
	fputs(buff2, stdout);
	fflush(stdout);

	pclose(wfp);
	pclose(rfp);
	close(w_fd);
	close(r_fd);
	close(stdin_fd);
	close(stdout_fd);
	return 0;
}
*/
