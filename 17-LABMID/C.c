//runner
#include "header.h"

int cless_sfd, sfd;
struct sockaddr_in serverAddress;
int ser_count, server;
struct serverStruct servers[MAXSER];


void getServers()
{
    //send enquiry:
    int len = sizeof(serverAddress);
    sendto(cless_sfd, enquiry, strlen(enquiry), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    //receive info
    recvfrom(cless_sfd, &ser_count, sizeof(ser_count), 0, (struct sockaddr *) &serverAddress, (socklen_t*)&len);
    recvfrom(cless_sfd, &servers, sizeof(servers), 0, (struct sockaddr *) &serverAddress, (socklen_t*)&len);

    //print available servers
    printServers(ser_count, servers);
}

int main()
{
    //start display
    printf("\n+---------------------+");
    printf("\n|     Client %5d    |", getpid());
    printf("\n|'enq' -> get list    |");
    printf("\n| # -> connect to port|");
    printf("\n+---------------------+\n");

    //open cless_sfd socket
    if ((cless_sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        error("Error in socket()\n");
    else printf("Successfully set up socket.\n");

    //setting up address for server
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(F_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //initial enquiry
    getServers();

    //loop
    char buff[MAXLEN];
    while (1) {
        clear(buff);
        printf("Enter => ");
        scanf("%s", buff);
        printf("Entered: %s\n", buff);
        printf("enquiry => %s\n", enquiry);

        if (strcmp(buff, enquiry) == 0)
            getServers();
        else if (strcmp(buff, "p") == 0)
            printServers(ser_count, servers);
        else {
            //port number entered
            int portno;
            if ((portno = atoi(buff)) == 0) {
                printf("Invalid port!\n");
                continue;
            }

            int found = 0;
            for (int i = 0; i < ser_count; i++)
                if (servers[i].PORT == portno) {
                    server = i;
                    found = 1;
                }
            if (found == 0) {
                printf("Sorry, that port has not been set up yet!\n");
                continue;
            }

            //open socket
            printf("Starting connection with F through port %d...\n", portno);
        	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        		error("Error in socket()\n");
        	else printf("Successfully set up socket.\n");

        	//get host name - because ipv4 is specified, function copies name into the h_name field
        	//and its struct in_addr equivalent into the h_addr_list[0] field of the returned hostent struct
        	struct hostent *hp;
        	hp = gethostbyname("127.0.0.1");
        	if (hp == NULL)
        		error("Error in gethostname()\n");
        	else printf("Got hostname.\n");

        	//setting up address
        	struct sockaddr_in address;
        	address.sin_family = AF_INET;
        	address.sin_port = htons(portno);

        	//copying hostname hp->h_addr into address.sin_addr
        	bcopy(hp->h_addr, (char *) &address.sin_addr, hp->h_length);	//returns NULL
        	printf("Successfully finished byte copy of hostname.\n");

        	//address we specify is the address of server we would like to comm. with
        	if ((connect(sfd, (struct sockaddr*) &address, sizeof(address))) < 0)
        		error("Error in connect()\n");

            printf("You can now use the service!\nEnter 'stop' to stop\n");
            while (1) {
                char buff[MAXLEN] = "";
                clear(buff);
                printf("Enter String To Convert => ");
                scanf("%s", buff);

                // clear message - will also send stop the server can stop servicing it
                if (send(sfd, buff, strlen(buff), 0) < 0)
                    error("Error in send()\n");

                //stop if needed to
                if (strcmp(buff, "stop") == 0) {
                    break;
                }

                clear(buff);
                //receive message
                int n;
                char msg[100];
                //while (1) {
                    if ((n = recv(sfd, &msg, sizeof(msg), 0)) < 0)
                        error("Error in recv()\n");
                    else if (n > 0) {
                        printf("Converted => %s\n", msg);
                            //break;
                        }
                //}
                clear(msg);
            }

            //close(sfd);

            //SEND REVIEW TO F:
            char review[MAXLEN];
            printf("Enter a review: ");
            fflush(stdout);
            read(0, &review, MAXLEN);

            int servernum = server;
            sendto(cless_sfd, reviewKey, strlen(reviewKey), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

            struct reviewStruct rev;
            rev.server = server;
            strcpy(rev.review, review);
            sendto(cless_sfd, &rev, sizeof(rev), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
            printf("Sent review to server %d!\n -> %s\n", rev.server, rev.review);
        }
    }

}
