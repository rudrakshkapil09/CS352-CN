//Echo Server - P2
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
    char c4[100] = "hip1";
	printf("\n-----------[P2]------------\n");
    char x[100];
    clear(x, 100);
    read(0, x, 100);
    printf("Message sent by Process P1 : %s\n",x);
    int fd = x[strlen(x)-1]-'0';
    dup2(fd, 1);
    printf("%s", c4);
    return 0;
}
