#include "header.h"

//global fds
int sfd;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(sfd, SHUT_RDWR);	//disallow further reads & writes
	close(sfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//MAIN
int main(int argc, char *args[])
{
	//cleanup
	signal(SIGINT, cleanup);

    //get pfno from command line args
    int pfno = atoi(args[1]);
    struct sockaddr_in pf_address, sm_address;
    socklen_t address_len = sizeof(pf_address);

    //start display
    printf("\n+------------------+");
    printf("\n|    Platform %d   |", pfno);
    printf("\n+------------------+\n");

    //create socket and get address acc. to pfno
    int ports[4] = {0, PF1_PORT, PF2_PORT, PF3_PORT};
    pf_address = createSocketDG(&sfd, ports[pfno]);

    //loop to get trains
    struct board B;
    while (1) {
        //get board from station
        recvfrom(sfd, &B, sizeof(B), 0, (struct sockaddr *) &sm_address, &address_len);
        printf("Train #%d coming to platform %d\n", B.trainno, B.pfno);
		fflush(stdout);

        //check if trains platform number matches it's own
        if (B.pfno == pfno) {
            //get coach sequence
            char buff[100];
			printf("Waiting for coach sequence...\n");
            recvfrom(sfd, buff, 100, 0, (struct sockaddr *) &sm_address, &address_len);
            printf("Coach sequence => %s\n", buff);

            //get board from train and send to station
			printf("Waiting for train to leave...\n");
            recvfrom(sfd, &B, sizeof(B), 0, (struct sockaddr *) &sm_address, &address_len);
			printf("Informing station...\n");
            setAddress(&sm_address, STATION_PORT);
            sendto(sfd, &B, sizeof(B), 0, (struct sockaddr *) &sm_address, sizeof(sm_address));
			printf("Done with 1 train\n\n");
        }
    }
}
