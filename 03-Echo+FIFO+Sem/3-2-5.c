//Broadcast Server - Receiver 3
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    char c[100];
    scanf("%s",c);
    printf("\n---------[In Receiver 3]---------");
    printf("\nMessage Received from Broadcast Server is : %s",c);
}
