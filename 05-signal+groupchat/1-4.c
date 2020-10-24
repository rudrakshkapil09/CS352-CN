//Signalling to a group using fcntl
//Client 3
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int pr = 1;

void hdfn(int x)
{
    printf("P3 - Recieved signal from pm\n");
    pr--;
}

int main()
{
    signal(SIGUSR1, hdfn);
    printf("P3 is running\n");
    while(pr);
    return 0;
}
