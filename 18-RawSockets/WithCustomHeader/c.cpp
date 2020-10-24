//client
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
    if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //client address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_CLIENT);

    if (bind(rsfd,(sockaddr*)&addr,sizeof(addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

    //loop
    cout << "Ready to receive.\n";
	while (1) {
		int sz;
        sockaddr_in saddr;
		char packet[PACKET_LEN];
        char buff[MAXLEN];
        memset(packet, 0, sizeof(packet));

		//recvfrom() is blocking
		if (recvfrom(rsfd,&packet,PACKET_LEN,0,(sockaddr*)&saddr,(socklen_t*)&sz) < 0)
			error("recvfrom()");

        //get ip header part
		struct ip *iphdr;
		iphdr = (struct ip*)packet;
        printIPH(iphdr);
        printf("Message => %s\n", packet+MSG_OFFSET);		//print the message
    }

	return 0;
}
