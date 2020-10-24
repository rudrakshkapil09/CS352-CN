//Station master
#include "header.h"

//global socket fds & status indicator for platforms
int hyd_sfd, vja_sfd, del_sfd, sm_sfd;    //last is for stationmaster
int status[3] = {0};    //0->empty

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(hyd_sfd, SHUT_RDWR);	//disallow further reads & writes
	close(hyd_sfd);
    shutdown(vja_sfd, SHUT_RDWR);	//disallow further reads & writes
    close(vja_sfd);
    shutdown(del_sfd, SHUT_RDWR);	//disallow further reads & writes
    close(del_sfd);
    shutdown(sm_sfd, SHUT_RDWR);	//disallow further reads & writes
	close(sm_sfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//MAIN
int main()
{
    //start display
    printf("\n+------------------+");
    printf("\n|  Station Master  |");
    printf("\n+------------------+\n");

    //cleanup on sigint
    signal(SIGINT, cleanup);

    //addresses and setup
    struct sockaddr_in vja_end, hyd_end, del_end, sm_address, pf1_address, pf2_address, pf3_address, t_address;
    socklen_t address_len = sizeof(t_address);
    setAddress(&pf1_address, PF1_PORT);
    setAddress(&pf2_address, PF2_PORT);
    setAddress(&pf3_address, PF3_PORT);

    //create sockets - also stores addresses in first param
    vja_end = createSocketDG(&vja_sfd, VJA_PORT);
    hyd_end = createSocketDG(&hyd_sfd, HYD_PORT);
    del_end = createSocketDG(&del_sfd, DEL_PORT);
    sm_address = createSocketDG(&sm_sfd, STATION_PORT);

    //select
    fd_set read_fds;
    while (1) {
        //init to 0
        FD_ZERO(&read_fds);

        //set fds
        FD_SET(vja_sfd, &read_fds);
        FD_SET(hyd_sfd, &read_fds);
        FD_SET(del_sfd, &read_fds);
        FD_SET(sm_sfd, &read_fds);

        //find max
        int max_fd = max(vja_sfd, hyd_sfd);
        max_fd = max(max_fd, max(del_sfd, sm_sfd));

        //select()
        int ret;
        if ((ret = select(max_fd+1, &read_fds, NULL, NULL, NULL)) <= 0) {
            printf("Select interrupted!\n");
            continue;
        }

        /* check which one has data to read */
        //insert all into an array of sfds and names
        int fds[3] = {hyd_sfd, vja_sfd, del_sfd};
        char names[3][20] = {"Hyderabad", "Vijayawada", "Delhi"};

        //Train is coming on
		printf("Checking ports...\n");
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(fds[i], &read_fds)) {
                printf("Train coming from %s...\n", names[i]);

                //find free platform
                int freePlatform = 0;
                for (; freePlatform < 3; freePlatform++) {
                    if (status[freePlatform] == 0) {
                        status[freePlatform] = 1;
                        break;
                    }
                }

                //get trainnumber in array and address in t_address
                int info[2];
                int p = recvfrom(fds[i], info, sizeof(info), 0, (struct sockaddr *) &t_address, &address_len);
                printf("Train number => %d\n", info[0]);
                int trainNumber = info[0];

                //send to platforms if any free
                if (freePlatform < 3) {
                    //create board
                    struct board B;
                    B.trainno = trainNumber;
                    B.pfno = (freePlatform+1);
                    printf("Train #%d coming to platform %d\n", B.trainno, B.pfno);
					fflush(stdout);
                    //set t address with port as info[1]
                    setAddress(&t_address, info[1]);

                    //send platform index to train
                    //if (sendto(fds[i], &freePlatform, sizeof(freePlatform), 0, (struct sockaddr *) &t_address, sizeof(t_address)) < 0)
                    //    error("Error in send()\n");
					struct platformNumberStruct pfs;
					pfs.num = freePlatform+1;
					sendto(sm_sfd, &pfs, sizeof(pfs), 0, (struct sockaddr *) &t_address, sizeof(t_address));

                    //send board to platforms
                    sendto(sm_sfd, &B, sizeof(B), 0, (struct sockaddr *) &pf1_address, sizeof(pf1_address));
                    sendto(sm_sfd, &B, sizeof(B), 0, (struct sockaddr *) &pf2_address, sizeof(pf2_address));
                    sendto(sm_sfd, &B, sizeof(B), 0, (struct sockaddr *) &pf3_address, sizeof(pf3_address));
					printf("Informed all parties\n\n");
                }
                else printf("No empty platforms!\n");
            }
        }

        //if train is ready to leave
        if (FD_ISSET(sm_sfd, &read_fds)) {
            struct board B;
            recvfrom(sm_sfd, &B, sizeof(B), 0, (struct sockaddr *) &sm_address, &address_len);
            status[B.pfno-1] = 0;
            printf("Train #%d ready to leave platform %d", B.trainno, B.pfno);
			fflush(stdout);
        }
    }
}
