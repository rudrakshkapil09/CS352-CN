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
    struct sockaddr_in a_addr = setAddress(IP_A);
	struct sockaddr_in b_addr = setAddress(IP_B);

    if (bind(rsfd,(sockaddr*)&a_addr,sizeof(a_addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

	//make IP header
	char packet[PACKET_LEN];
	memset(packet, 0, sizeof(packet));
	struct ip* hdr = makeIPHeader(150, IP_A, IP_B, packet);

    //loop
	const char *signature = " - from A";
	while (1) {
        cout << "Enter: ";
        char buf[MAXLEN];
		clear(buf);
		scanf("%s", buf);

		//sendto() is non blocking
		strcat(buf, signature);
		strcpy(packet+MSG_OFFSET, buf);
		printf("Sending: '%s'...\n", packet+MSG_OFFSET);
		if (sendto(rsfd,&packet,sizeof(packet),0,(sockaddr*)&b_addr,sizeof(b_addr)) < 0)
		      error("sendto");
	}
}
