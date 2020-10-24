//2-2-2.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
		char message[1000];
		int i;
		read(0, message, 1000);	//TO GET SPACE SEPARRTED STRING
		strcat(message, " + [This section added by P2]\n");
		write(1, message, 1000);
		//printf("%s\n", message);
}
