//server
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
	if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));

    //server address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP_SERVER);

    if (bind(rsfd,(sockaddr*)&addr,sizeof(addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

    //client address
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(IP_CLIENT);

    //loop
	while (1) {
        cout << "Enter: ";
        char buf[MAXLEN];
		clear(buf);
		scanf("%s", buf);

		//sendto() is non blocking
		if (sendto(rsfd,&buf,strlen(buf),0,(sockaddr*)&client_addr,sizeof(client_addr)) < 0)
		      error("sendto");
	}
}
