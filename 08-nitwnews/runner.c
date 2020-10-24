#include <stdio.h>
#include <stdlib.h>

int main()
{
    system("gcc editor.c -o editor");
    system("gcc reporter.c -o reporter");
    system("gcc documentwriter.c -o documentwriter");
    system("gcc newsreader.c -o newsreader");
    system("gcc telecaster.c -o telecaster");
}
