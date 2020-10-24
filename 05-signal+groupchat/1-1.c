#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

int main()
{
    int c = getpid(), c1, c2, c3;
    int pp[2];
    pipe(pp);

    char *args[1];
    args[0] = NULL;
    if((c1 = fork()) == 0)
        execvp("./1-2", args);
    if((c2 = fork()) == 0)
        execvp("./1-3", args);
    if((c3 = fork()) == 0)
        execvp("./1-4", args);

    setpgid(0, 0);
    setpgid(c1, c);
    setpgid(c2, c);
    setpgid(c3, c);
    printf("After setting pgids:\n");
    printf("PID: %d %d %d %d\n", c, c1, c2, c3);
    printf("PGID: %d %d %d %d\n", getpgid(c), getpgid(c1), getpgid(c2), getpgid(c3));

    int gid = getpgid(c);
    sleep(1);
    fcntl(pp[0], F_SETFL, O_ASYNC);
    //fcntl(pp[0], __F_SETSIG, SIGUSR1);
    fcntl(pp[0], F_SETOWN, -gid);

    write(pp[1], "bruh ", 4);
    close(pp[1]);
    
    killpg(gid, SIGUSR1);
    wait(NULL);
    return 0;
}
