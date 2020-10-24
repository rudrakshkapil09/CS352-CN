#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void openNewTerminal(char *program)
{
    char cmd[200], path[100];
    sprintf(cmd, "osascript -e 'tell app \"Terminal\" to do script \"%s/%s\"'", getcwd(path, 100), program);
    system(cmd);

    /*
    system("osascript -e 'tell app \"Terminal\" to activate'");
    system(path);
    system(program);
    printf("Opened!\n");
    system("osascript -e 'tell app \"Terminal\" to do script \"%s\"'", program);
    */
}

int main()
{
    openNewTerminal("2");
    while (1)
        ;
}
