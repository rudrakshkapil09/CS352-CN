//Signalling to a group using fcntl
//Client 2
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int pr = 1;

void hdfn(int x)
{
    printf("P2 - Recieved signal from pm\n");
    pr--;
}

int main()
{
    signal(SIGUSR1, hdfn);
    printf("P2 is running\n");
    while(pr);
    return 0;
}
