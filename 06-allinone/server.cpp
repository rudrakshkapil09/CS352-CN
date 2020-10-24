//ALL IN ONE
//Server
//Contains code for c1 as well
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <signal.h>
#include <vector>
using namespace std;

//globals
char ffifo[21] = "/tmp/famousfifo";
char myfifo1[21] = "/tmp/";
char pfifo[21] = "/tmp/pfifo";
char ppfifo[21] = "/tmp/ppfifo";
vector <string> ppid;
int s = 0;

//handler for SIGUSR1
void hdfn(int signo) {
    //open echo for client if any exists
    if (ppid.size() != 0) {
        /*FOR MAC*/
        char a[] = "osascript -e 'tell app \"Terminal\" to do script \"./echo ";
        /*FOR WINDOWS*/
        //char a[] = "gnome-terminal -x ./echo ";
        char b[100];
        strcpy(b, ppid[s].c_str());
        s = (s+1) % ppid.size();
        strcat(a, b);
        strcat(a, "\"'");         /*FOR MAC*/
        system(a);
    }
}

//MAIN
int main() {
    int pfd[2];
    pipe(pfd);
    int c = fork();
    pollfd ppfd[5];

    if (c > 0) {
        close(pfd[1]);
        mkfifo(ffifo, 0666);
        mkfifo(pfifo, 0666);
        mkfifo(ppfifo, 0666);

        int serfd = open(ppfifo, O_WRONLY);
        int p = (int) getpid();
        char c[100];
        sprintf(c, "%d", p);
        write(serfd, c, strlen(c)+1);
        cout << "Writing PID done: " << c << endl;
        close(serfd);

        int sfd = open(ffifo,O_RDONLY |O_NONBLOCK);
        int ffd = open(pfifo,O_RDONLY | O_NONBLOCK);

        int fd = fileno(popen("./p3", "r"));
        //int outfd = fileno(popen("./p5", "w"));
        signal(SIGUSR1, hdfn);
        int wfd = dup(1);

        //set up poll
        ppfd[0].fd = 0;
        ppfd[1].fd = pfd[0];
        ppfd[2].fd = ffd;
        ppfd[3].fd = fd;
        ppfd[4].fd = sfd;
        ppfd[0].events = POLLIN;
        ppfd[1].events = POLLIN;
        ppfd[2].events = POLLIN;
        ppfd[3].events = POLLIN;
        ppfd[4].events = POLLIN;

        while (1) {
            poll(ppfd, 5, 3);

            //key board input
            if (ppfd[0].revents & POLLIN) {
                char a[100];
                cin >> a;

                //send message to clients
                for (int i = 0; i < ppid.size(); i++) {
                    char add[20] = "/tmp/";
                    strcat(add, ppid[i].c_str());

                    cout << "Send To " << add << " " << a <<endl;
                    cout << "Sent By Keyboard " << endl;

                    int fd4 = open(add, O_WRONLY);
                    write(fd4, a, strlen(a)+1);
                    fflush(stdout);
                    close(fd4);
                }
            }

            //Input from p1
            if (ppfd[1].revents & POLLIN ) {
                char a[100];
                read(pfd[0], a, 100);
                for (int i = 0; i < ppid.size(); i++) {   //--------------------------------msg from client----------------------------
                    char add[20] = "/tmp/";
                    strcat(add, ppid[i].c_str());

                    cout << "Sent To " << add << " " << a << endl;
                    cout << "Sent By P1 " << endl;

                    int fd4 = open(add, O_WRONLY);
                    write(fd4, a, strlen(a)+1);
                    fflush(stdout);
                    close(fd4);
                }
            }

            // Input from p2
            if (ppfd[2].revents & POLLIN ) {
                char a[100];
                read(ffd, a, 100);
                for (int i = 0; i < ppid.size(); i++) {   //--------------------------------msg from client----------------------------
                    char add[20] = "/tmp/";
                    strcat(add, ppid[i].c_str());

                    cout << "Sent To " << add << " " << a <<endl;
                    cout << "Sent By P2" << endl;

                    int fd4 = open(add, O_WRONLY);
                    write(fd4, a, strlen(a)+1);
                    fflush(stdout);
                    close(fd4);
                }
            }

            // Input from p3
            if (ppfd[3].revents & POLLIN ) {
                char a[100];
                read(fd, a, 100);

                for (int i = 0; i < ppid.size(); i++) {   //--------------------------------msg from client----------------------------
                    char add[20] = "/tmp/";
                    strcat(add, ppid[i].c_str());

                    cout << "Sent To " << add << " " << a <<endl;
                    cout << "Sent By P3" << endl;

                    int fd4 = open(add, O_WRONLY);
                    write(fd4, a, strlen(a)+1);
                    fflush(stdout);
                    close(fd4);
                }
            }

            //Clien to clients
            if (ppfd[4].revents & POLLIN ) {
                char b[100];
                char pid[20];
                char msg[100];

                //get message and pid of client p
                read(sfd, b, 100);
                if (strlen(b) > 0 && b[0] != '\0') {
                    char* token = strtok(b, ",");
                    strcpy(pid, token);
                    token = strtok(NULL, ",");
                    strcpy(msg, token);
                    cout << msg << endl;
                }
                fflush(stdout);
                string p(pid);
                cout << "pid " << p << endl;

                //For new client p
                if (find(ppid.begin(), ppid.end(), p) == ppid.end()) {
                    cout << "Not found\n";
                    ppid.push_back(p);
                    char add[20] = "/tmp/";
                    strcat(add, pid);
                    mkfifo(add, 0666);
                    int fd4 = open(add, O_WRONLY);
                    write(fd4, "Welcome", 8);
                    close(fd4);
                }

                //message from client p to others
                for (int i = 0; i < ppid.size(); i++) {
                    if (ppid[i] != p) {
                        char add[20] = "/tmp/";
                        strcat(add, ppid[i].c_str());
                        cout << "Sent To " << add << " " << msg << endl;
                        cout << "Sent By Client" << endl;
                        int fd4 = open(add, O_WRONLY);
                        write(fd4, msg, strlen(msg)+1);
                        fflush(stdout);
                        close(fd4);
                    }
                }
            }
        }
    }
    else { //P1
        close(pfd[0]);
        while (1) {
            sleep(10);
            char a[100] = "Message from P1";
            write(pfd[1], a, sizeof(a));
        }
    }
    return 0;
}
