//client
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
    printf("\n+-----------+");
    printf("\n|     B     |");
    printf("\n+-----------+\n");
    printf("Will receive messages only from A\n");

    if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //set addresses
	struct sockaddr_in b_addr = setAddress(IP_B);

    if (bind(rsfd,(sockaddr*)&b_addr,sizeof(b_addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

    //loop
    cout << "Ready to receive.\n";
	while (1) {
        int sz;
        sockaddr_in saddr;
		char packet[PACKET_LEN];
        memset(packet, 0, sizeof(packet));

		//recvfrom() is blocking
		if (recvfrom(rsfd,&packet,sizeof(packet),0,(sockaddr*)&saddr,(socklen_t*)&sz) < 0)
			error("recvfrom()");

        //get ip header part
		struct ip *iphdr;
		iphdr = (struct ip*)packet;
        printIPH(iphdr);

        //check if header was correct
        if (strcmp(inet_ntoa(iphdr->ip_src), IP_A) == 0)
            printf("Message => %s\n", packet+(iphdr->ip_hl*4));			//print the message
        else printf("NOT ALLOWED -> %s\n", inet_ntoa(iphdr->ip_src));
	}

	return 0;
}
