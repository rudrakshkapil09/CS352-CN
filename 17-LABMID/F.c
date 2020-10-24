//runner
#include "header.h"

//globals
int cless_sfd;
int ser_count = 0;
struct serverStruct servers[MAXSER];
int map[MAXSER];
int mapcounts[MAXSER];

struct msg_ds
{
	long msg_type;
	char msg_data[MAXLEN];
} m;

//utility
void cleanup()
{
    shutdown(cless_sfd, SHUT_RDWR);
    close(cless_sfd);
    printf("\n-------------[END]-----------------\n");
    exit(0);
}


int main()
{
    signal(SIGINT, cleanup);

    for (int i = 0; i < MAXSER; i++)
        mapcounts[i] = 0;

    //start display
    printf("\n+--------------------+");
    printf("\n|  Facilitator %5d |", getpid());
    printf("\n+--------------------+\n");

    /* MSGQ stuff */
    key_t key;
    int msq_id;

    //get key of IPC and get message Q id
    if ((key = ftok("./", 100)) == -1)
        error("Error in Path\n");

    //get the queue after clearing old one
    msq_id = msgget(key, 0666 | IPC_CREAT);
    printf("Message Queue Created\nID: %d\n", msq_id);

    //create connectionless sfd for listening to clients
    struct sockaddr_in f_address, s_address, c_address;
    socklen_t f_len, s_len, address_len = sizeof(f_address);
    f_address = createSocketDG(&cless_sfd, F_PORT);

    //select: - connectionless, keyboard and service sfds
    fd_set readfds;
    while (1) {
        FD_ZERO(&readfds);

        int maxFD = cless_sfd;
        FD_SET(0, &readfds);
        FD_SET(cless_sfd, &readfds);
        for (int i = 0; i < ser_count; i++) {
            FD_SET(servers[i].sfd, &readfds);
            maxFD = max(servers[i].sfd, maxFD);
        }

        if (select(maxFD+1, &readfds, NULL, NULL, NULL) <= 0)
            continue;

        //keyboard:
        if (FD_ISSET(0, &readfds)) {
            if (ser_count >= MAXSER) {
                printf("max servers reached!\n");
                continue;
            }

            char buff[MAXLEN];
            read(0, buff, MAXLEN);

            printf("Entered through keyboard => %s\n", buff);
            printf("Creating process...");

            //get port and path
            char *tok;
            tok = strtok(buff, " ");
            int portno;
            if ((portno = atoi(tok)) == 0) {
                printf("Invalid port!\n");
                continue;
            }
            else {
                int found = 0;
                for (int i = 0; i < ser_count; i++) {
                    if (portno == servers[i].PORT) {
                        printf("Port already in use!\n");
                        found = 1;
                    }
                }
                if (found)
                    continue;
            }
            tok = strtok(NULL, "\n");    //path

            //create socket
            int sfd;
            s_address = createSocket(&sfd, portno);

            servers[ser_count].PORT = portno;
            servers[ser_count].sfd = sfd;
            servers[ser_count].number = ser_count+1;

            printf("Starting process %s...\n", tok);
            int c = fork();
            if (c == 0) {
                //we want write to still work, so only send via 0
                dup2(sfd, 0);

                //actually start service
                execlp(tok, NULL);
            }
            else {
                servers[ser_count].pid = c;
                ser_count++;
                printf("Process created!\n");
                printServers(ser_count, servers);
            }
        }

        //connectionless - for client to get info
        if (FD_ISSET(cless_sfd, &readfds)) {
            char buff[MAXLEN];
            clear(buff);
            int len = sizeof(c_address);
            recvfrom(cless_sfd, &buff, sizeof(buff), 0, (struct sockaddr *) &c_address, (socklen_t*)&len);
            printf("Received from client => %s\n", buff);

            if (strcmp(enquiry, buff) == 0) {
                printf("Sending info to client...\n");
                //send back count and then servers
                sendto(cless_sfd, &ser_count, sizeof(ser_count), 0, (struct sockaddr *) &c_address, address_len);
                sendto(cless_sfd, &servers, sizeof(servers), 0, (struct sockaddr *) &c_address, address_len);
            }

            else if (strcmp(buff, reviewKey) == 0) {
                //put in review msgq & inform si
                struct reviewStruct rev;
                recvfrom(cless_sfd, &rev, sizeof(rev), 0, (struct sockaddr *) &c_address, (socklen_t*)&len);

                //put in message q
                printf("%d\n", rev.server);
                m.msg_type = servers[rev.server].pid;
                clear(m.msg_data);
                sprintf(m.msg_data, "%s", rev.review);
                assert(msgsnd(msq_id, &m, sizeof(m), 0) == 0);
            }
        }

        //check all service sfds
        for (int i = 0; i < ser_count; i++) {
            //client wants to connect with server i
            if (FD_ISSET(servers[i].sfd, &readfds)) {
                //notify the service server:
                printf("Notifying server %d...\n", (i+1));
                kill(servers[i].pid, SIGUSR1);
                break;
            }
        }
    }
}
