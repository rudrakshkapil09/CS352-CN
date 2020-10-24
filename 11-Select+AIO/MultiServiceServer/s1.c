//Service 1 - Converts to Lowercase
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
const int x = 32;

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < 100; i++)
		buff[i] = '\0';
}

//MAIN
int main()
{
	char buff[100];
	while (1) {
		clear(buff);
		recv(0, &buff, sizeof(buff), 0);
		//read(0, buff, 100);

		for (int i = 0; buff[i] != '\0'; i++)
			buff[i] = buff[i] | x;

		send(1, buff, strlen(buff), 0);
		//write(1, buff, strlen(buff)+1);
	}
 	return 0;
}
