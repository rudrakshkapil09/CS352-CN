//ALL IN ONE
//Echo Server - created by S to communicate with client
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <signal.h>
using namespace std;

int main(int argc,char *argv[])
{
    int p;
    if ((p = system("g++ server.cpp -o server")) < 0)
        perror("System perror Server\n");

    if ((p = system("g++ p2.cpp -o p2")) < 0)
        perror("System error P2\n");

    if ((p = system("g++ p3.cpp -o p3")) < 0)
        perror("System error P3\n");

    if ((p = system("g++ p4.cpp -o p4")) < 0)
        perror("System error P4\n");

    if ((p = system("g++ p5.cpp -o p5")) < 0)
        perror("System error P5\n");

    if ((p = system("g++ echo.cpp -o echo")) < 0)
        perror("System fault Echo\n");

	if ((p = system("g++ clientc1.cpp -o clientc1")) < 0)
		perror("System fault Client\n");
}
