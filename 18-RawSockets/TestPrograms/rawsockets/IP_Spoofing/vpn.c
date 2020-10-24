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
	struct sockaddr_in addr,serv_addr,cl_addr;int len = sizeof(cl_addr);
	memset(&addr,0,sizeof(addr));
	memset(&serv_addr,0,sizeof(addr));
	memset(&cl_addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.5");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	if(bind(rsfd,(struct sockaddr*)&addr,sizeof(addr))<0)
	{
		perror("Could not bind");
	}
	else
		printf("Bound..\n");
	char buffer[1024];
	struct MyIPheader *ip;
	ip = (struct MyIPheader*)buffer;//int len = sizeof(cl_addr);
	while(1)
	{
		if(recvfrom(rsfd,buffer,1024,0,(struct sockaddr*)&cl_addr,&len)<0)
			perror("Could not read");
		else
		{
			printf("Read.\n");
			ip->ttl = 64;
			ip->protocol = 253;
			ip->id = 0;
			ip->check = 0;
			ip->saddr = inet_addr("127.0.0.5");
			ip->daddr = inet_addr("127.0.0.2");
			if(sendto(rsfd,buffer,1024,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
				perror("Could not send");
			else
			{
				if(recvfrom(rsfd,buffer,1024,0,NULL,NULL)<0)
					perror("Could not receive from server");
				else
				{
					ip->ttl = 64;
					ip->id = 0;
					ip->protocol = 253;
					ip->check = 0;
					ip->saddr = inet_addr("127.0.0.5");
					ip->daddr = cl_addr.sin_addr.s_addr;
					cl_addr.sin_port = 0;
					if(sendto(rsfd,buffer,1024,0,(struct sockaddr*)&cl_addr,sizeof(cl_addr))<0)
						perror("Could not send");
				}
			}
		}
	}
	close(rsfd);
	return 0;
}
