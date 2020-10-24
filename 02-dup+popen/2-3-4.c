//P1 P2 P3 -> cascading output redirection
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    char buff[100];
    char id[2] = "4";
    //fgets(buff, 100, stdin);
    scanf("%s", buff);
    strcat(buff, id);
    //printf("P3: %s\n", buff);
    int fp = fileno(fopen("2-3output.txt", "w"));
    write(fp, buff, strlen(buff));
}
