//NEWS
//document writer - created by editor when it wants output to file
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//MAIN
int main(int argc, char *argv[])
{
    FILE *fp = fopen("./document.txt", "a");

    fprintf(fp, "%s", argv[0]);
    fclose(fp);

    return 0;
}
