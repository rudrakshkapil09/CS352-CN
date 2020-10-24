//client
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
    if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));

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
		char buf[MAXLEN];
        clear(buf);

		//recvfrom() is blocking
		if (recvfrom(rsfd,&buf,sizeof(buf),0,(sockaddr*)&saddr,(socklen_t*)&sz) < 0)
			error("recvfrom()");


        //get ip header part
		struct ip *iphdr;
		iphdr = (struct ip*)buf;
        printIPH(iphdr);
		printf("Message => %s\n", buf+(iphdr->ip_len)*4);		//print the message
	}

	return 0;
}
