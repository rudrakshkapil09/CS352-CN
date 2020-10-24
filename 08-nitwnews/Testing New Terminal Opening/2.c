#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    printf("Hello world!\n");
    while (1) {
        char inp[10];
        printf("Enter: ");
        scanf("%s", inp);
        printf("Input => %s\n", inp);
        if (strcmp(inp, "exit") == 0) {
            printf("Quitting...");
            break;
        }
    }
    system("exit");
    system("exit");
}
