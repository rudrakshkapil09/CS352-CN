#include "networks.h"
#define BUF_LEN 160
int main(int argc, char const *argv[])
{
	int rsfd = socket(AF_INET,SOCK_RAW,253);
	int optval = 1;
	setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));//IP_HDRINCL
	if(rsfd<0)
	{
		perror("Could not create socket");exit(0);
	}
	struct sockaddr_in addr,addr1;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	if(bind(rsfd,(struct sockaddr*)&addr,sizeof(addr))<0)
	{
		perror("Could not bind");
	}
	// accept doesn't work in raw sockets
	char buffer[BUF_LEN],buff[BUF_LEN];
	int sz;
	int len = sizeof(addr1);
	if((sz = recvfrom(rsfd,buffer,BUF_LEN,0,(struct sockaddr*)&addr1,&len))<0)
	{
		perror("Could not read");
	}
	else
	{
		struct iphdr *ip;char ad[INET_ADDRSTRLEN];
  	ip=(struct iphdr*)buffer;
		//cout<<(buf+(ip->ihl)*4)<<endl;
//		printf("TTL: %d\nVersion: %d\nProtocol: %d\nForeign IP: %s\n",(int)ip->ttl,(int)ip->version,(int)ip->protocol,inet_ntop(AF_INET,&(ip->saddr),ad,INET_ADDRSTRLEN));
		for(int i=0;i<ip->ihl*4;i++)
		printf("%d %c\n",i,buffer[i]);
		strcpy(buff,buffer+(ip->ihl)*4);
		strcat(buff,"127.0.0.2\n");
		printf("Path:\n%s\n",buff);
	}
	return 0;
}
