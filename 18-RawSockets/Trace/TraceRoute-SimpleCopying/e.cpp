//B -> Will be sending to C after receiving from C
#include "header.h"

int rsfd;


int main(int argc, char **argv)
{
	printf("\n+-----------+");
	printf("\n|     E     |");
	printf("\n+-----------+\n");
	printf("Will print full trace route\n");

	if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //set addresses
    struct sockaddr_in this_addr = setAddress(IP_E);

    if (bind(rsfd,(sockaddr*)&this_addr,sizeof(this_addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

    //receive message
    int sz;
    sockaddr_in saddr;
    char packet[PACKET_LEN];
    memset(packet, 0, sizeof(packet));

    //recvfrom() is blocking
    if (recvfrom(rsfd,&packet,sizeof(packet),0,(sockaddr*)&saddr,(socklen_t*)&sz) < 0)
        error("recvfrom()");

    //print all IP's
    printAllIPH(packet,4);
    printf("End of trace route!\n");
}
