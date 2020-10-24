#include "header.h"

//global sfd
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

int main()
{
    //cleanup
    signal(SIGINT, cleanup);

    //addresses
    struct sockaddr_in my_address, sm_address, pf_address;
    socklen_t address_len = sizeof(my_address), pf_len = sizeof(pf_address);

    //get train details
    int trainNumber, myport;
    char pointOfArrival, coachSequence[100];
    while (1) {
        printf("Enter point of arrival (D/V/H): ");
        scanf("%c", &pointOfArrival);
        if (pointOfArrival == 'D' || pointOfArrival == 'V' || pointOfArrival == 'H')
            break;
    }
    printf("Enter train number: ");
    scanf("%d", &trainNumber);
    printf("Enter coach sequence: ");
    scanf("%s", coachSequence);

    //start display
    printf("\n\n+=-----------------+");
    printf("\n|      Train %d    |", trainNumber);
    printf("\n+------------------+\n");

    //get port number
    switch (pointOfArrival) {
        case 'D': myport = DEL_PORT; break;
        case 'H': myport = HYD_PORT; break;
        case 'V': myport = VJA_PORT; break;
    }

    //set address of staion using myport
    setAddress(&sm_address, myport);

    //create socket
    my_address = createSocketDG(&sfd, getpid());

    //send information to station master
    int info[2];
    info[0] = trainNumber;
    info[1] = ntohs(my_address.sin_port);
    sendto(sfd, info, sizeof(info), 0, (struct sockaddr *) &sm_address, sizeof(sm_address));
    printf("Sent details to station - [%d] -+- [%d]\n", info[0], info[1]);

    //get platform number from station
	struct platformNumberStruct pfs;
    pfs.num = -1;
	printf("Waiting for platform number...\n");
    recvfrom(sfd, &pfs, sizeof(pfs), 0, (struct sockaddr *) &my_address, &address_len);
	int platformNumber = pfs.num;
    if (platformNumber == -1)
        printf("No free platforms :(\n");
    else printf("Platform => %d\n", platformNumber);
	fflush(stdout);

    //send coach sequence to platform
    int ports[4] = {0, PF1_PORT, PF2_PORT, PF3_PORT};
    setAddress(&pf_address, ports[platformNumber]);
    sendto(sfd, coachSequence, sizeof(coachSequence)+1, 0, (struct sockaddr *) &pf_address, sizeof(pf_address));
    printf("Coach sequence sent!\n");

    //For train leaving
    printf("Enter G when the train is about to leave:\n");
	//fflush(stdout);
    char c;
    while (1) {
        scanf("%c", &c);
		//printf("%c\n", c);

        //send board to station
        if (c == 'G') {
            struct board B;
            B.pfno = platformNumber;
            B.trainno = trainNumber;
            sendto(sfd, &B, sizeof(B), 0, (struct sockaddr *) &pf_address, sizeof(pf_address));
            printf("Train left!\n");
            break;
        }
    }
}
