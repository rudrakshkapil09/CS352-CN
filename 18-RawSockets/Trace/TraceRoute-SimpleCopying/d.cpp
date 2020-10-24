//B -> Will be sending to C after receiving from C
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
	printf("\n+-----------+");
	printf("\n|     D     |");
	printf("\n+-----------+\n");
	printf("Will send messages to E\n");

	if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //set addresses
    struct sockaddr_in this_addr = setAddress(IP_D);
	struct sockaddr_in send_addr = setAddress(IP_E);

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

    //make own IP header
    char packet2[PACKET_LEN];
    memset(packet2, 0, sizeof(packet2));
    struct ip* hdr = makeIPHeader(150, IP_D, IP_E, packet2);

    //put previous header into message
    strcpy(packet2+MSG_OFFSET, packet+MSG_OFFSET);
    strcpy(packet2+MSG_OFFSET*2, packet+MSG_OFFSET*2);
    strcpy(packet2+MSG_OFFSET*3, packet+MSG_OFFSET*3);
    printAllIPH(packet2,3);

    //send to next
    strcpy(packet2+MSG_OFFSET*4, IP_D);
    printAllIPH(packet2,4);
    printf("(E will get this)\n\n");
	printf("Sending Packet to E...\n");
	if (sendto(rsfd,&packet2,sizeof(packet2),0,(sockaddr*)&send_addr,sizeof(send_addr)) < 0)
	      error("sendto");
}
