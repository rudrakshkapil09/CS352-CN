//server
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
	if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //server address
    struct sockaddr_in addr = setAddress(IP_SERVER);
	struct sockaddr_in c_addr = setAddress(IP_CLIENT);

    if (bind(rsfd,(sockaddr*)&addr,sizeof(addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

	//make IP header
	char packet[PACKET_LEN];
	memset(packet, 0, sizeof(packet));
	struct ip* hdr = makeIPHeader(150, IP_SERVER, IP_CLIENT, packet);
	printIPH(hdr);

    //loop
	while (1) {
        cout << "Enter: ";
        char buf[MAXLEN];
		clear(buf);
		scanf("%s", buf);

		//sendto() is non blocking
		strcpy(packet+(MSG_OFFSET), buf);
		printf("Sending: '%s'...\n", packet+(MSG_OFFSET));
		if (sendto(rsfd,&packet,PACKET_LEN,0,(sockaddr*)&c_addr,sizeof(c_addr)) < 0)
		      error("sendto");
	}
}
