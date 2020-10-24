//A -> Will be sending to B
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
	printf("\n+-----------+");
	printf("\n|     A     |");
	printf("\n+-----------+\n");
	printf("Will send messages to B\n");

	if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //set addresses
    struct sockaddr_in this_addr = setAddress(IP_A);
	struct sockaddr_in send_addr = setAddress(IP_B);

    if (bind(rsfd,(sockaddr*)&this_addr,sizeof(this_addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

	//make IP header
	char packet[PACKET_LEN];
	memset(packet, 0, sizeof(packet));
	struct ip* hdr = makeIPHeader(150, IP_A, IP_B, packet);

	//sendto() is non blocking
	int x;
	printf("Type something to get started...\n");
	cin >> x;

	strcpy(packet+MSG_OFFSET, IP_A);
	printAllIPH(packet,1);
	printf("(B will get this)\n\n");
	printf("Sending Packet to B...\n");
	if (sendto(rsfd,&packet,sizeof(packet),0,(sockaddr*)&send_addr,sizeof(send_addr)) < 0)
	      error("sendto");
}
