//ALL IN ONE
//P5 - output of server
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
using namespace std;

int main() {
    char buff[100];
    while(1) {
        cin >> buff;
        cout << buff << endl;
    }
    return 0;
}
