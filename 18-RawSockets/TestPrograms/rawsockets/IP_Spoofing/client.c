#include "networks.h"

int main(int argc, char const *argv[])
{
	int rsfd = socket(AF_INET,SOCK_RAW,253);
	if(rsfd<0)
	{
		perror("could not create socket");exit(0);
	}
	int op = 1;
	setsockopt(rsfd,IPPROTO_IP,IP_HDRINCL,&op,sizeof(op));
	struct sockaddr_in addr,serv_addr;//int len = sizeof(serv_addr);
	memset(&addr,0,sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.3");

	if(bind(rsfd,(struct sockaddr*)&addr,sizeof(addr))<0)
	{
		perror("Could not bind");
	}
	else
		printf("Bound..\n");
	char buffer[1024];
	struct MyIPheader *ip;
	ip = (struct MyIPheader*)buffer;
	char ad[INET_ADDRSTRLEN];
	while(1)
	{
		ip->version = 4;
		ip->protocol = 253;
		ip->ttl = 64;
		ip->id = 0;
		//ip->check = 0;
		ip->saddr = inet_addr("127.0.0.3");

		printf("Enter an address\n");
		scanf("%s",ad);
		memset(&serv_addr,0,sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(ad);
		ip->daddr = inet_addr(ad);
		ip->ihl = 5;
		if(sendto(rsfd,buffer,1024,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
			perror("Could not send");
		else
			printf("Success..\n");
		if(recvfrom(rsfd,buffer,1024,0,NULL,NULL)<0)
			perror("Could not receive");
		else
			printf("%s\n",buffer+(ip->ihl*4));
	}
	close(rsfd);
	return 0;
}
