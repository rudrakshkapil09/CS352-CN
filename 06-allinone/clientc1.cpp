//ALL IN ONE
//Client
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
using namespace std;

//globals
int echoFlag = 0;
char myfifo1[21] = "/tmp/famousfifo";
char myfifo2[21] = "/tmp/";
char myfifo3[21] = "/tmp/";
int fd1,fd2, efd;
char buffA[100];
char buffB[100];

//handler for SIGUSR1 - toggles echoFlag and displays
void hdfn(int signo) {
    echoFlag = (echoFlag + 1) % 2;
    if (echoFlag % 2)
        cout << "ECHO => CONNECTED\nServer => Disconnected\n";
    else cout << "SERVER => CONNECTED\nEcho => Disconnected\n";
}

//MAIN
int main() {
    //get pid of the client in char[] form
    char pidStr[20];
    string str = to_string(getpid());
    strcpy(pidStr, str.c_str());

    //make fifo 1 - famousfifo
    mkfifo(myfifo1 ,0666);

    //make fifo 2 - server
    strcat(myfifo2, pidStr);
    cout << "Reader " << myfifo2 << endl;
    mkfifo(myfifo2, 0666);

    //make fifo 3 - echo
    strcat(myfifo3, "1");
    strcat(myfifo3, pidStr);
    mkfifo(myfifo3, 0666);

    //open fifo 2 and 3 - 1 is opened in write only mode when needed
    fd2 = open(myfifo2, O_RDONLY | O_NONBLOCK);
    efd = open(myfifo3, O_RDONLY | O_NONBLOCK);

    //make poll
    pollfd pfd[3];
    pfd[0].fd = 0;                //Keyboard
    pfd[1].fd = fd2;              //fifo 2
    pfd[2].fd = efd;              //fifo 3
    pfd[0].events = POLLIN;
    pfd[1].events = POLLIN;
    pfd[2].events = POLLIN;
    cout << "Poll Creation Done!" << endl;

    //set handler of SIGUSR1 as hdfn
    signal(SIGUSR1, hdfn);

    //continuously take input by polling every 0.003 seconds
    while (1) {
        int r = poll(pfd, 3, 3);

        //from keyboard
        if (pfd[0].revents & POLLIN) {
            //get input message into buffA
            fgets(buffA, 80, stdin);

            //get pid in char[] form
            char pidStr[20];
            string str = to_string(getpid());
            strcpy(pidStr, str.c_str());

            //append pid to the message
            strcat(pidStr, ",");
            strcat(pidStr, buffA);
            strcpy(buffA, pidStr);

            //open famousfifo and send message
            fd1 = open(myfifo1, O_WRONLY);
      	    write(fd1, buffA, strlen(buffA)+1);
            close(fd1);
            sleep(1);
         }

         //from fifo 2 - server
         if ((pfd[1].revents & POLLIN) && (echoFlag % 2 == 0)) {
             read(fd2, buffB, 100);
             if (buffB[0] != '\0') {
                 cout << buffB << endl;
                 buffB[0] = '\0';
             }
             fflush(stdout);
         }

         //from fifo 3 - echo
         if ((pfd[2].revents & POLLIN ) && (echoFlag % 2 != 0)) {
             char buffB[100];
             buffB[0]='\0';
            read(efd,buffB,100);
            if(buffB[0]!='\0') {
                cout<<buffB<<endl;
                buffB[0]='\0';
            }
            fflush(stdout);
      }
    }
  return 0;
}
